#include <irc/strv.h>

#include <stdlib.h>
#include <string.h>

strv_t strv_new(void)
{
    return calloc(1, sizeof(struct strv_));
}

void strv_free(strv_t v)
{
    size_t s = 0;

    if (v == NULL) {
        return;
    }

    if (v->v != NULL) {
        for (s = 0; s < v->vlen; s++) {
            free(v->v[s]);
        }
        free(v->v);
        v->v = NULL;
    }

    free(v);
}

ssize_t strv_add(strv_t v, char const *s)
{
    char *copy = NULL;
    char **tmp = NULL;

    if (s != NULL) {
        copy = strdup(s);
        if (copy == NULL) {
            return -1;
        }
    }

    tmp = recallocarray(v->v,
                        v->vlen,
                        v->vlen + 1,
                        sizeof(char*)
        );
    if (tmp == NULL) {
        return -1;
    }

    v->v = tmp;
    v->v[v->vlen] = copy;
    ++v->vlen;

    return v->vlen;
}

void strv_dump(strv_t v)
{
    size_t i = 0;

    printf("[len = %lu]\n", v->vlen);
    if (v->v != NULL) {
        for (; i < v->vlen; i++) {
            printf("  [%lu] = \"%s\"\n", i,
                   (v->v[i] == NULL ? "NULL" : v->v[i])
                );
        }
    }
}
