#include <irc/irc.h>
#include <irc/strbuf.h>
#include <irc/util.h>
#include <irc/message.h>
#include <irc/queue.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    char cmd[100];
    irc_command_handler_t handler;
    void *arg;
} irc_handler_t;

typedef enum {
    irc_state_unknown = 0,
    irc_state_connected,
    irc_state_userline,
    irc_state_ping,

    irc_state_ready,
} irc_state_t;

struct irc_
{
    strbuf_t buf;
    pthread_mutex_t buffermtx;

    char *nick;
    char hostname[100];
    char *realname;
    char *server;

    irc_handler_t *handler;
    size_t handlerlen;

    irc_state_t state;

    pthread_mutex_t sendqmtx;
    irc_queue_t sendq;

    irc_queue_t channels;
};

static void irc_ping_handler(irc_t i, irc_message_t m, void *unused)
{
    /* queue pong
     */
    irc_queue_command(i, "PONG", NULL);
}

static void irc_invite_handler(irc_t i, irc_message_t m, void *unused)
{
    char *channel = NULL;

    if (m->args == NULL || m->argslen < 1) {
        return;
    }

    channel = m->args[1];
    irc_join(i, channel);
}

irc_t irc_new(void)
{
    irc_t i = NULL;
    int r = 0;

    i = calloc(1, sizeof(struct irc_));
    if (i == NULL) {
        return NULL;
    }

    i->buf = strbuf_new();
    if (i->buf == NULL) {
        free(i);
        return NULL;
    }

    i->sendq = irc_queue_new();
    if (i->sendq == NULL) {
        irc_free(i);
        return NULL;
    }

    i->channels = irc_queue_new();
    if (i->channels == NULL) {
        irc_free(i);
        return NULL;
    }

    /* determine hostname
     */
    r = gethostname(i->hostname, sizeof(i->hostname)-1);
    if (r < 0 || i->hostname[0] == '\0') {
        strlcpy(i->hostname, "unknown", sizeof(i->hostname));
    }

    pthread_mutex_init(&i->buffermtx, NULL);
    pthread_mutex_init(&i->sendqmtx, NULL);

    irc_handler_add(i, "PING", irc_ping_handler, NULL);
    irc_handler_add(i, "INVITE", irc_invite_handler, NULL);

    return i;
}

void irc_free(irc_t i)
{
    return_if_true(i == NULL,);

    pthread_mutex_lock(&i->buffermtx);
    pthread_mutex_destroy(&i->buffermtx);

    free(i->handler);
    strbuf_free(i->buf);
    free(i->nick);
    free(i->realname);
    free(i->server);

    irc_queue_free(i->channels);

    pthread_mutex_lock(&i->sendqmtx);
    pthread_mutex_destroy(&i->sendqmtx);
    irc_queue_free(i->sendq);

    free(i);
}

irc_error_t irc_reset(irc_t i)
{
    pthread_mutex_lock(&i->sendqmtx);
    irc_queue_clear(i->sendq, (free_t)irc_message_free);
    pthread_mutex_unlock(&i->sendqmtx);

    irc_queue_clear(i->channels, (free_t)free);
    strbuf_reset(i->buf);

    i->state = irc_state_unknown;

    return irc_error_success;
}

irc_error_t irc_setopt(irc_t i, ircopt_t o, ...)
{
    va_list lst;
    irc_error_t e = irc_error_success;

    va_start(lst, o);
    switch (o) {
    case ircopt_nick:
    {
        free(i->nick);
        i->nick = strdup(va_arg(lst, char*));
    } break;

    case ircopt_server:
    {
        free(i->server);
        i->server = strdup(va_arg(lst, char*));
    } break;

    case ircopt_realname:
    {
        free(i->realname);
        i->realname = strdup(va_arg(lst, char*));
    } break;

    default: e = irc_error_argument; break;

    }
    va_end(lst);

    return e;
}

irc_error_t irc_feed(irc_t i, char const *buffer, size_t len)
{
    ssize_t ret = 0;

    return_if_true(i == NULL || buffer == NULL, irc_error_argument);

    pthread_mutex_lock(&i->buffermtx);
    ret = strbuf_append(i->buf, buffer, len);
    pthread_mutex_unlock(&i->buffermtx);

    if (ret < 0) {
        return irc_error_memory;
    }

    return irc_error_success;
}

static irc_error_t irc_think_data(irc_t i)
{
    ssize_t s = 0;
    char *line = NULL;
    size_t linesize = 0;
    irc_error_t r = irc_error_internal;
    irc_message_t m = NULL;
    int idx = 0;

    pthread_mutex_lock(&i->buffermtx);
    s = strbuf_getstr(i->buf, &line, &linesize, "\r\n");
    pthread_mutex_unlock(&i->buffermtx);

    if (s < 0) {
        r = irc_error_success;
        goto cleanup;
    }

    /* strip \r\n from the line
     */
    if (linesize > 2) {
        line[linesize-2] = '\0';
        linesize -= 2;
    }

    m = irc_message_new();
    if (m == NULL) {
        r = irc_error_memory;
        goto cleanup;
    }

    r = irc_message_parse(m, line, linesize);
    if (IRC_FAILED(r)) {
        goto cleanup;
    }

    for (idx = 0; idx < i->handlerlen; idx++) {
        irc_handler_t *h = i->handler + idx;

        if (strlen(h->cmd) == 0 ||
            strcmp(h->cmd, m->command) == 0) {
            h->handler(i, m, h->arg);
        }
    }

    r = irc_error_success;

cleanup:

    free(line);
    line = NULL;

    irc_message_free(m);
    m = NULL;

    return r;
}

static void irc_check_data(irc_t i)
{
    if (i->realname == NULL) {
        i->realname = strdup(i->nick);
    }

    if (i->server == NULL) {
        i->server = strdup("unknown");
    }
}

irc_error_t irc_think(irc_t i)
{
    irc_error_t r = irc_error_internal;

    return_if_true(i == NULL, irc_error_argument);

    if (i->state == irc_state_unknown ||
        i->state == irc_state_connected) {
        irc_check_data(i);
        r = irc_queue_command(
            i, "USER",
            i->nick, i->hostname, i->server, i->realname,
            NULL
            );
        if (IRC_FAILED(r)) {
            return r;
        }
        i->state = irc_state_userline;
    }

    if (i->state == irc_state_userline) {
        r = irc_queue_command(i, "NICK", i->nick, NULL);
        if (IRC_FAILED(r)) {
            return r;
        }
        i->state = irc_state_ready;
    }

    r = irc_think_data(i);
    return r;
}

irc_error_t irc_queue(irc_t i, irc_message_t m)
{
    irc_error_t r;

    return_if_true(i == NULL || m == NULL, irc_error_argument);

    pthread_mutex_lock(&i->sendqmtx);
    r = irc_queue_push(i->sendq, m);
    pthread_mutex_unlock(&i->sendqmtx);

    return r;
}

irc_error_t irc_queue_command(irc_t i, char const *command, ...)
{
    irc_message_t m = NULL;
    va_list lst;

    return_if_true(i == NULL || command == NULL, irc_error_argument);

    va_start(lst, command);
    m = irc_message_makev(i->nick, command, lst);
    va_end(lst);

    if (m == NULL) {
        return irc_error_memory;
    }

    return irc_queue(i, m);
}

irc_error_t irc_handler_add(irc_t i, char const *cmd,
                            irc_command_handler_t handler,
                            void *arg)
{
    irc_handler_t *tmp = NULL;

    tmp = recallocarray(i->handler, i->handlerlen,
                        i->handlerlen+1, sizeof(irc_handler_t)
        );
    if (!tmp) {
        return irc_error_memory;
    }

    i->handler = tmp;

    i->handler[i->handlerlen].arg = arg;
    i->handler[i->handlerlen].handler = handler;
    if (cmd != NULL) {
        strncpy(i->handler[i->handlerlen].cmd, cmd,
                sizeof(i->handler[i->handlerlen].cmd)
            );
    }
    ++i->handlerlen;

    return irc_error_success;
}

irc_error_t irc_pop(irc_t i, char **message, size_t *len)
{
    irc_message_t msg = NULL;
    irc_error_t r;

    return_if_true(i == NULL, irc_error_argument);

    pthread_mutex_lock(&i->sendqmtx);
    msg = irc_queue_pop(i->sendq);
    pthread_mutex_unlock(&i->sendqmtx);

    return_if_true(msg == NULL, irc_error_nodata);

    r = irc_message_string(msg, message, len);
    irc_message_free(msg);

    return r;
}

irc_error_t irc_connected(irc_t i)
{
    return_if_true(i == NULL, irc_error_argument);

    i->state = irc_state_connected;

    return irc_error_success;
}

irc_error_t irc_join(irc_t i, char const *channel)
{
    char *chan = NULL;
    irc_error_t e;

    return_if_true(i == NULL, irc_error_argument);
    return_if_true(channel == NULL, irc_error_argument);

    e = irc_queue_command(i, "JOIN", channel, NULL);
    if (IRC_FAILED(e)) {
        return e;
    }

    chan = strdup(channel);
    irc_queue_push(i->channels, chan);

    return irc_error_success;
}
