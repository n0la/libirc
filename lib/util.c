#include <irc/util.h>

irc_error_t irc_strv_add(char ***strv, size_t *argslen, char *what)
{
    char **tmp = NULL;
    size_t add = 2;

    if (strv == NULL || argslen == NULL || what == NULL) {
        return irc_error_argument;
    }

    tmp = recallocarray(*strv, *argslen, (*argslen)+add, sizeof(char*));
    if (tmp == NULL) {
        return irc_error_memory;
    }

    tmp[*argslen] = what;
    tmp[(*argslen)+1] = NULL;
    *argslen += 1;

    *strv = tmp;

    return irc_error_success;
}

void irc_strv_free(char **strv)
{
    char **tmp = NULL;

    if (strv == NULL) {
        return;
    }

    for (tmp = strv; *tmp != NULL; ++tmp) {
        free(*tmp);
        *tmp = NULL;
    }
    free(strv);
}
