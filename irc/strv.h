#ifndef LIBIRC_STRV_H
#define LIBIRC_STRV_H

#include <stdio.h>
#include <string.h>

struct strv_ {
    size_t vlen;
    char **v;
};

typedef struct strv_ *strv_t;

strv_t strv_new(void);
void strv_free(strv_t v);

ssize_t strv_add(strv_t v, char const *s);
void strv_dump(strv_t v);

#endif
