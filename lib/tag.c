#include <irc/tag.h>
#include <irc/strbuf.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

irc_tag_t irc_tag_new(void)
{
    irc_tag_t t = NULL;

    t = calloc(1, sizeof(struct irc_tag_));
    if (!t) {
        return NULL;
    }

    return t;
}

void irc_tag_free(irc_tag_t t)
{
    if (t == NULL) {
        return;
    }

    free(t->key);
    t->key = NULL;

    free(t->value);
    t->value = NULL;

    free(t);
}

irc_error_t irc_tag_parse(irc_tag_t t, const char *str)
{
    char *ptr = NULL;
    char *value = NULL;
    char *key = NULL;

    if (t == NULL || str == NULL) {
        return irc_error_argument;
    }

    value = strdup(str);
    if (value == NULL) {
        return irc_error_memory;
    }
    ptr = value;

    key = strsep(&value, "=");
    t->key = strdup(key);
    t->value = irc_tag_unescape(value);

    free(ptr);

    return irc_error_success;
}

char *irc_tag_unescape(const char *value)
{
    char *ret = NULL;
    strbuf_t unescaped = NULL;

    unescaped = strbuf_new();

    while (value && value[0]) {
        switch(value[0]) {
        case '\\':
            value++;
            switch(value[0]) {
            case ':':
                strbuf_append(unescaped, ";", 1);
                value++;
                break;
            case 's':;
                strbuf_append(unescaped, " ", 1);
                value++;
                break;
            case '\\':
                strbuf_append(unescaped, "\\", 1);
                value++;
                break;
            case 'r':
                strbuf_append(unescaped, "\r", 1);
                value++;
                break;
            case 'n':
                strbuf_append(unescaped, "\n", 1);
                value++;
                break;
            default:
                if (value[0]) {
                    strbuf_append(unescaped, &value[0], 1);
                    value++;
                }
                break;
            }
            break;
        default:
            strbuf_append(unescaped, &value[0], 1);
            value++;
            break;
        }
    }

    if (strbuf_len(unescaped) > 0) {
        ret = strbuf_strdup(unescaped);
    }
    strbuf_free(unescaped);

    return ret;
}
