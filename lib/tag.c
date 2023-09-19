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

irc_error_t irc_tag_parse(irc_tag_t t, char const *str)
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

irc_error_t irc_tag_string(irc_tag_t t, char **s, size_t *slen)
{
    strbuf_t buf = NULL;

    if (t == NULL || t->key == NULL || s == NULL) {
        return irc_error_argument;
    }

    buf = strbuf_new();
    strbuf_append(buf, t->key, strlen(t->key));
    if (t->value != NULL) {
        char *value = irc_tag_escape(t->value);

        strbuf_append(buf, "=", 1);
        strbuf_append(buf, value, strlen(value));
        free(value);
    }

    *s = strbuf_strdup (buf);
    if (slen != NULL) {
        *slen = strbuf_len(buf);
    }
    strbuf_free(buf);

    return irc_error_success;
}

char *irc_tag_unescape(char const *value)
{
    char *ret = NULL;
    strbuf_t unescaped = NULL;

    unescaped = strbuf_new();

    while (value && value[0]) {
        switch (value[0]) {
        case '\\':
            value++;
            switch (value[0]) {
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

char *irc_tag_escape(char const *value)
{
    char *ret = NULL;
    strbuf_t escaped = NULL;

    escaped = strbuf_new();

    while (value && value[0]) {
        switch (value[0]) {
        case ';':
            strbuf_append(escaped, "\\:", 2);
            break;
        case ' ':
            strbuf_append(escaped, "\\s", 2);
            break;
        case '\\':
            strbuf_append(escaped, "\\\\", 2);
            break;
        case '\r':
            strbuf_append(escaped, "\\r", 2);
            break;
        case '\n':
            strbuf_append(escaped, "\\n", 2);
            break;
        default:
            strbuf_append(escaped, value, 1);
            break;
        }
        value++;
    }

    ret = strbuf_strdup(escaped);
    strbuf_free(escaped);

    return ret;
}
