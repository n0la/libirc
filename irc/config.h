#ifndef LIBIRC_CONFIG_H
#define LIBIRC_CONFIG_H

#include <irc/error.h>
#include <irc/pa.h>

#include <stdint.h>
#include <stdbool.h>

struct irc_config_;
typedef struct irc_config_ *irc_config_t;

struct irc_config_network_;
typedef struct irc_config_network_ *irc_config_network_t;

irc_config_t irc_config_new(void);
void irc_config_free(irc_config_t config);

char const *irc_config_error_string(irc_config_t c);

pa_t irc_config_networks(irc_config_t c);
irc_error_t irc_config_load_file(irc_config_t c, char const *filename);
irc_error_t irc_config_add_network(irc_config_t c, irc_config_network_t n);

irc_config_network_t irc_config_network_new(char const *name);
void irc_config_network_unref(irc_config_network_t n);
void irc_config_network_ref(irc_config_network_t n);

void irc_config_network_set_host(irc_config_network_t n, char const *h);
void irc_config_network_set_port(irc_config_network_t n, char const *p);
void irc_config_network_set_nick(irc_config_network_t n, char const *p);
void irc_config_network_set_nickserv(irc_config_network_t n, char const *value);
void irc_config_network_set_nickserv_password(irc_config_network_t n,
                                              char const *value);
void irc_config_network_set_ssl(irc_config_network_t n, bool v);

char const *irc_config_network_host(irc_config_network_t n);
char const *irc_config_network_port(irc_config_network_t n);
char const *irc_config_network_nick(irc_config_network_t n);
char const *irc_config_network_nickserv(irc_config_network_t n);
char const *irc_config_network_nickserv_password(irc_config_network_t n);
bool irc_config_network_ssl(irc_config_network_t n);

#endif
