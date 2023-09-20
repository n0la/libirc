#ifndef LIBIRC_TAG_H
#define LIBIRC_TAG_H

#include <irc/error.h>

#include <stddef.h>

struct irc_tag_
{
    char *key;
    char *value;
};
typedef struct irc_tag_ *irc_tag_t;

irc_tag_t irc_tag_new(void);
void irc_tag_free(irc_tag_t t);

irc_error_t irc_tag_parse(irc_tag_t t, char const *str);

char *irc_tag_unescape(char const *value);

#endif
