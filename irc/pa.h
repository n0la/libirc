#ifndef LIBIRC_PA_H
#define LIBIRC_PA_H

#include <stdio.h>
#include <string.h>

typedef void (*free_t)(void*);

struct pa_ {
    size_t vlen;
    void **v;
    free_t free;
};

typedef struct pa_ *pa_t;

pa_t pa_new(void);
pa_t pa_new_full(free_t free);
void pa_free(pa_t v);

ssize_t pa_add(pa_t v, void *s);

#endif
