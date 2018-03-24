#ifndef LIBIRC_UTIL_H
#define LIBIRC_UTIL_H

#include <irc/error.h>

#include <stdlib.h>
#include <stdio.h>

extern irc_error_t irc_strv_add(char ***strv, size_t *len, char *what);
extern void irc_strv_free(char **strv);

#endif
