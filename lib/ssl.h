#ifndef LIBIRC_SSL_H
#define LIBIRC_SSL_H

#include <irc/error.h>
#include <stdint.h>
#include <unistd.h>

void *irc_ssl_client_new(void);
void irc_ssl_client_free(void *arg);
irc_error_t irc_ssl_client_connect(void *arg, int sock, char const *host);
irc_error_t irc_ssl_client_disconnect(void *arg);
int irc_ssl_client_read(void *arg, void *buffer, size_t);
int irc_ssl_client_write(void *arg, void const *buffer, size_t);

#endif
