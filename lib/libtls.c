#include "ssl.h"

#include <tls.h>
#include <stdlib.h>

typedef struct {
    struct tls *tls;
    struct tls_config *tls_config;
} libtls_t;

void *irc_ssl_client_new(void)
{
    libtls_t *p = calloc(1, sizeof(libtls_t));

    p->tls_config = tls_config_new();
    if (p->tls_config == NULL) {
        irc_ssl_client_free(p);
        return NULL;
    }

    p->tls = tls_client();
    if (p->tls == NULL) {
        irc_ssl_client_free(p);
        return NULL;
    }

    tls_configure(p->tls, p->tls_config);

    return p;
}

void irc_ssl_client_free(void *arg)
{
    libtls_t *p = (libtls_t*)arg;

    if (p->tls != NULL) {
        tls_close(p->tls);
        tls_free(p->tls);
        p->tls = NULL;
    }

    if (p->tls_config != NULL) {
        tls_config_free(p->tls_config);
        p->tls_config = NULL;
    }

    free(p);
}

irc_error_t irc_ssl_client_disconnect(void *arg)
{
    libtls_t *p = (libtls_t*)arg;

    if (p->tls == NULL) {
        return irc_error_success;
    }

    tls_close(p->tls);
    tls_free(p->tls);
    p->tls = NULL;

    return irc_error_success;
}

irc_error_t irc_ssl_client_connect(void *arg, int sock, char const *host)
{
    libtls_t *c = (libtls_t*)arg;
    int ret = 0;

    if (c->tls != NULL) {
        return irc_error_success;
    }

    c->tls = tls_client();
    if (c->tls == NULL) {
        return irc_error_tls;
    }

    ret = tls_connect_socket(c->tls, sock, host);
    if (ret < 0) {
        return irc_error_tls;
    }

    ret = tls_handshake(c->tls);
    if (ret < 0) {
        return irc_error_tls;
    }

    return irc_error_success;
}

int irc_ssl_client_read(void *arg, void *buffer, size_t size)
{
    libtls_t *c = (libtls_t*)arg;
    int ret = 0;

    return_if_true(c == NULL || c->tls == NULL, -1);

    do {
        ret = tls_read(c->tls, buffer, size);
        if (ret == TLS_WANT_POLLIN || ret == TLS_WANT_POLLOUT) {
            continue;
        }
    } while (0);

    return ret;
}

int irc_ssl_client_write(void *arg, void const *buffer, size_t size)
{
    libtls_t *c = (libtls_t*)arg;
    int ret = 0;

    return_if_true(c == NULL || c->tls == NULL, -1);

    do {
        ret = tls_write(c->tls, buffer, size);
        if (ret == TLS_WANT_POLLIN || ret == TLS_WANT_POLLOUT) {
            continue;
        }
    } while (0);

    return ret;
}
