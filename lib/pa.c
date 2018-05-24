#define _GNU_SOURCE
#include <irc/pa.h>
#include <stdlib.h>
#include <string.h>

pa_t pa_new(void)
{
    return pa_new_full((free_t)free);
}

pa_t pa_new_full(free_t ff)
{
    pa_t p = calloc(1, sizeof(struct pa_));

    if (p == NULL) {
        return NULL;
    }

    p->free = ff;

    return p;
}

void pa_free(pa_t v)
{
    size_t s = 0;

    if (v == NULL) {
        return;
    }

    if (v->v != NULL) {
        for (s = 0; s < v->vlen; s++) {
            v->free(v->v[s]);
        }
        free(v->v);
        v->v = NULL;
    }

    free(v);
}

ssize_t pa_add(pa_t v, void *s)
{
    void **tmp = NULL;

    tmp = reallocarray(v->v, v->vlen + 1, sizeof(void*));
    if (tmp == NULL) {
        return -1;
    }

    v->v = tmp;
    v->v[v->vlen] = s;
    ++v->vlen;

    return v->vlen;
}
