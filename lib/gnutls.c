#include "ssl.h"

#include <gnutls/gnutls.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

static bool global_init = false;

typedef struct {
    bool init;
    gnutls_session_t session;
    gnutls_certificate_credentials_t xcred;
} gnutls_t;

void *irc_ssl_client_new(void)
{
    gnutls_t *p = calloc(1, sizeof(gnutls_t));

    if (global_init == false) {
        gnutls_global_init();
        global_init = true;
    }

    if (p == NULL) {
        return NULL;
    }

    /* X509 stuff */
    gnutls_certificate_allocate_credentials(&p->xcred);
    /* sets the system trusted CAs for Internet PKI */
    gnutls_certificate_set_x509_system_trust(p->xcred);

    return p;
}

void irc_ssl_client_free(void *arg)
{
    gnutls_t *p = (gnutls_t*)arg;

    if (p->init == true) {
        irc_ssl_client_disconnect(arg);
    }

    gnutls_certificate_free_credentials(p->xcred);

    free(p);
}

irc_error_t irc_ssl_client_disconnect(void *arg)
{
    gnutls_t *p = (gnutls_t*)arg;

    if (p->init == false) {
        return irc_error_internal;
    }

    gnutls_bye(p->session, GNUTLS_SHUT_RDWR);
    gnutls_deinit(p->session);
    p->init = false;

    return irc_error_success;
}

irc_error_t irc_ssl_client_connect(void *arg, int sock, char const *host)
{
    gnutls_t *p = (gnutls_t*)arg;
    int ret = 0;

    if (p->init == true) {
        return irc_error_internal;
    }

    /* Initialize TLS session */
    gnutls_init(&p->session, GNUTLS_CLIENT);
    /* Use default priorities */
    gnutls_set_default_priority(p->session);
    /* put the x509 credentials to the current session */
    gnutls_credentials_set(p->session, GNUTLS_CRD_CERTIFICATE, p->xcred);

    gnutls_server_name_set(p->session, GNUTLS_NAME_DNS, host, strlen(host));
    gnutls_session_set_verify_cert(p->session, host, 0);

    gnutls_transport_set_int(p->session, sock);

    /* Perform the TLS handshake */
    do {
        ret = gnutls_handshake(p->session);
    } while (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN);

    if (ret < 0) {
        gnutls_deinit(p->session);
        return irc_error_tls;
    }

    p->init = true;

    return irc_error_success;
}

int irc_ssl_client_read(void *arg, void *buffer, size_t size)
{
    gnutls_t *p = (gnutls_t*)arg;
    int ret = 0;

    do {
        ret = gnutls_record_recv(p->session, buffer, size);
    } while (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN);

    return ret;
}

int irc_ssl_client_write(void *arg, void const *buffer, size_t size)
{
    gnutls_t *p = (gnutls_t*)arg;
    int ret = 0;

    do {
        ret = gnutls_record_send(p->session, buffer, size);
    } while (ret == GNUTLS_E_INTERRUPTED || ret == GNUTLS_E_AGAIN);

    return ret;
}
