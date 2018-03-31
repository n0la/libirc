#include <irc/client.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <unistd.h>
#include <errno.h>
#include <getopt.h>

#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <tls.h>

struct irc_client_
{
    irc_t irc;
    char const *hostname;
    char const *port;
    bool ssl;
    int fd;

    void *addr;
    size_t addrlen;

    struct tls *tls;
    struct tls_config *tls_config;
};

irc_client_t irc_client_new(void)
{
    irc_client_t c = NULL;

    c = calloc(1, sizeof(struct irc_client_));
    if (c == NULL) {
        return NULL;
    }

    c->irc = irc_new();
    if (c->irc == NULL) {
        free(c);
        return NULL;
    }

    c->tls_config = tls_config_new();
    if (c->tls_config == NULL) {
        free(c);
        return NULL;
    }

    c->fd = -1;

    return c;
}

void irc_client_free(irc_client_t c)
{
    return_if_true(c == NULL,);

    irc_free(c->irc);
    c->irc = NULL;

    free(c);
}

irc_t irc_client_irc(irc_client_t c)
{
    return_if_true(c == NULL, NULL);
    return c->irc;
}

irc_error_t irc_client_disconnect(irc_client_t c)
{
    return_if_true(c->fd == -1, irc_error_success);

    close(c->fd);
    c->fd = -1;

    free(c->addr);
    c->addr = NULL;
    c->addrlen = 0;

    if (c->tls != NULL) {
        tls_close(c->tls);
        tls_free(c->tls);
        c->tls = NULL;
    }

    return irc_error_success;
}

irc_error_t irc_client_connect(irc_client_t c,
                               char const *host, char const *port,
                               bool ssl)
{
    struct addrinfo *info = NULL, *ai = NULL, hint = {0};
    int ret = 0, sock = 0;

    if (c->fd != -1) {
        return irc_error_success;
    }

    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_UNSPEC;
    hint.ai_flags = AI_PASSIVE;

    if ((ret = getaddrinfo(host, port, &hint, &info))) {
        return irc_error_internal;
    }

    for (ai = info; ai != NULL; ai = ai->ai_next) {
        sock = socket(ai->ai_family, SOCK_STREAM, 0);
        if (sock == -1) {
            continue;
        }

        if (connect(sock, ai->ai_addr, ai->ai_addrlen) == 0) {
            break;
        }

        close(sock);
        sock = -1;
    }

    if (sock < 0) {
        return irc_error_connection;
    }

    /* make an internal copy of the address we connected to
     */
    c->addr = calloc(1, ai->ai_addrlen);
    if (c->addr == NULL) {
        return irc_error_memory;
    }
    memcpy(c->addr, ai->ai_addr, ai->ai_addrlen);
    c->addrlen = ai->ai_addrlen;

    c->fd = sock;
    c->ssl = ssl;

    /* TODO: SSL with libtls
     */
    if (ssl) {
        c->tls = tls_client();
        if (c->tls == NULL) {
            irc_client_disconnect(c);
            return irc_error_tls;
        }

        /* configure the tls handle
         */
        tls_configure(c->tls, c->tls_config);

        /* do a TLS handshake
         */
        ret = tls_connect_socket(c->tls, c->fd, host);
        if (ret < 0) {
            irc_client_disconnect(c);
            return irc_error_tls;
        }
    }

    return irc_error_success;
}

int irc_client_read(irc_client_t c, void *buffer, size_t len)
{
    if (c->fd == -1) {
        return -1;
    }

    if (c->ssl && c->tls == NULL) {
        return -1;
    }

    if (c->ssl) {
        return tls_read(c->tls, buffer, len);
    } else {
        return read(c->fd, buffer, len);
    }
}

int irc_client_write(irc_client_t c, void const *buffer, size_t len)
{
    if (c->fd == -1) {
        return -1;
    }

    if (c->ssl && c->tls == NULL) {
        return -1;
    }

    if (c->ssl) {
        return tls_write(c->tls, buffer, len);
    } else {
        return write(c->fd, buffer, len);
    }
}
