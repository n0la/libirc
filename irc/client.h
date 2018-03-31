#ifndef LIBIRC_CLIENT_H
#define LIBIRC_CLIENT_H

#include <irc/irc.h>
#include <irc/error.h>
#include <irc/config.h>
#include <stdbool.h>

struct irc_client_;
typedef struct irc_client_ *irc_client_t;

irc_client_t irc_client_new(void);
irc_client_t irc_client_new_config(irc_config_network_t n);
void irc_client_free(irc_client_t c);

irc_t irc_client_irc(irc_client_t c);

bool irc_client_connected(irc_client_t c);
irc_error_t irc_client_disconnect(irc_client_t c);
irc_error_t irc_client_connect(irc_client_t c);
irc_error_t irc_client_connect2(irc_client_t c,
                                char const *host, char const *port,
                                bool usessl);

int irc_client_read(irc_client_t c, void *buffer, size_t len);
int irc_client_write(irc_client_t c, void const *buffer, size_t len);

#endif
