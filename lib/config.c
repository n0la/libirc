#include <irc/config.h>
#include <irc/pa.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern int yylex_init_extra(void *extra, void **state);
extern int yylex_destroy(void *state);
extern void yylex(void *state);
extern void yy_switch_to_buffer(void *buffer, void *scanner);
extern void *yy_scan_string(char const *s, void *scanner);
extern void yy_delete_buffer(void *b, void *scanner);
extern void yyset_in(FILE *f, void *scanner);
extern int yyparse(void *scanner, irc_config_t config);

struct irc_config_network_
{
    char *name;
    char *host;
    char *port;
    char *nick;
    bool ssl;
};

struct irc_config_
{
    char *error;
    pa_t networks;
    irc_config_network_t last;
};

irc_config_t irc_config_new(void)
{
    irc_config_t c = NULL;

    c = calloc(1, sizeof(struct irc_config_));
    if (c == NULL) {
        return NULL;
    }

    c->networks = pa_new_full((free_t)irc_config_network_free);
    if (c->networks == NULL) {
        free(c);
        return NULL;
    }

    return c;
}

void irc_config_set_error_string(irc_config_t c, char const *err)
{
    free(c->error);
    c->error = strdup(err);
}

char const *irc_config_error_string(irc_config_t c)
{
    return_if_true(c == NULL, NULL);
    return c->error;
}

pa_t irc_config_networks(irc_config_t c)
{
    if (c == NULL) {
        return NULL;
    }

    return c->networks;
}

irc_error_t irc_config_load_file(irc_config_t c, char const *filename)
{
    void *scanner = NULL;
    FILE *f = NULL;
    int ret = 0;

    if (c == NULL) {
        return irc_error_argument;
    }

    f = fopen(filename, "r");
    if (f == NULL) {
        return irc_error_io;
    }

    yylex_init_extra(c, &scanner);
    yyset_in(f, scanner);

    ret = yyparse(scanner, c);
    yylex_destroy(scanner);

    fclose(f);
    f = NULL;

    if (ret) {
        return irc_error_parse;
    }

    return irc_error_success;
}

void irc_config_free(irc_config_t config)
{
    if (config == NULL) {
        return;
    }

    pa_free(config->networks);
    config->networks = NULL;

    free(config);
}

irc_config_network_t irc_config_last(irc_config_t c)
{
    return c->last;
}

irc_error_t irc_config_add_network(irc_config_t c, irc_config_network_t n)
{
    if (c == NULL || n == NULL) {
        return irc_error_argument;
    }

    pa_add(c->networks, n);
    c->last = n;

    return irc_error_success;
}

irc_config_network_t irc_config_network_new(char const *name)
{
    irc_config_network_t n = NULL;

    if (name == NULL) {
        return NULL;
    }

    n = calloc(1, sizeof(struct irc_config_network_));
    if (n == NULL) {
        return NULL;
    }

    n->name = strdup(name);
    n->ssl = true;

    return n;
}

void irc_config_network_set_host(irc_config_network_t n, char const *h)
{
    free(n->host);
    n->host = strdup(h);
}

void irc_config_network_set_port(irc_config_network_t n, char const *p)
{
    free(n->port);
    n->port = strdup(p);
}

void irc_config_network_set_nick(irc_config_network_t n, char const *p)
{
    free(n->nick);
    n->nick = strdup(p);
}

void irc_config_network_set_ssl(irc_config_network_t n, bool v)
{
    n->ssl = v;
}

char const *irc_config_network_host(irc_config_network_t n)
{
    return_if_true(n == NULL, NULL);
    return n->host;
}

char const *irc_config_network_port(irc_config_network_t n)
{
    return_if_true(n == NULL, NULL);
    return n->port;
}

char const *irc_config_network_nick(irc_config_network_t n)
{
    return_if_true(n == NULL, NULL);
    return n->nick;
}

bool irc_config_network_ssl(irc_config_network_t n)
{
    return_if_true(n == NULL, false);
    return n->ssl;
}

void irc_config_network_free(irc_config_network_t n)
{
    if (n == NULL) {
        return;
    }

    free(n->name);
    free(n->host);
    free(n->port);
    free(n->nick);

    free(n);
}
