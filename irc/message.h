#ifndef LIBIRC_MESSAGE_H
#define LIBIRC_MESSAGE_H

#include <irc/error.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#define IRC_PROTOCOL_DELIMITER "\r\n"

#define IRC_COMMAND_MODE           "MODE"
#define IRC_COMMAND_NICK           "NICK"
#define IRC_COMMAND_PRIVMSG        "PRIVMSG"

#define IRC_ERR_NICKNAMEINUSE      "433"

struct irc_message_
{
    int ref;
    char *prefix;
    char *command;
    char **args;
    size_t argslen;
};

typedef struct irc_message_ *irc_message_t;

irc_message_t irc_message_new(void);

void irc_message_unref(irc_message_t m);
void irc_message_ref(irc_message_t m);

irc_message_t irc_message_parse2(char const *line, size_t linesize);

irc_error_t irc_message_parse(irc_message_t m,
                              char const *line,
                              size_t lensize);

irc_message_t irc_message_privmsg(char const *prefix,
                                  char const *target,
                                  char const *msg, ...);

irc_message_t irc_message_make(char const *prefix,
                               char const *command, ...);

irc_message_t irc_message_makev(char const *prefix,
                                char const *command,
                                va_list lst);

irc_error_t irc_message_string(irc_message_t m, char **s, size_t *slen);

bool irc_message_is(irc_message_t m, char const *cmd);
bool irc_message_prefix_nick(irc_message_t m, char const *nick);

#endif
