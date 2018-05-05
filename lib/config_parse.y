%define api.pure full

%parse-param {void *scanner} {irc_config_t config}
%lex-param {void *scanner}

%{
#include <irc/config.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern int yylex(void *lval, void *scanner);

/* private
 */
extern irc_config_network_t irc_config_last(irc_config_t c);
extern void irc_config_set_error_string(irc_config_t c, char const *err);

void yyerror(void *scanner, irc_config_t config, char const *err)
{
    irc_config_set_error_string(config, err);
}

int yywrap(void)
{
    return 1;
}

char *strip_quote(char *s)
{
    size_t len = strlen(s);

    if (len < 1) {
        return s;
    }

    if (*s == '"') {
        memmove(s, s+1, len-1);
        --len;
    }

    if (len > 0 && s[len-1] == '"') {
        s[len-1] = '\0';
    }

    return s;
}

%}

%union {
    char *string;
    int integer;
    double number;
}

%token TOK_PAR_OPEN TOK_PAR_CLOSE TOK_EQUAL
%token TOK_SEMI_COLON TOK_COMMENT TOK_NETWORK
%token  <string>        TOK_QUOTED_STRING
%token  <string>        TOK_STRING

%%
config: /* empty */
        |       config network
        |       config comment
        ;

network_start:  TOK_NETWORK TOK_QUOTED_STRING TOK_PAR_OPEN
                {
                    irc_config_network_t net = irc_config_network_new($2);
                    free($2);
                    if (net == NULL) {
                        YYERROR;
                    }
                    if (irc_config_add_network(config, net)) {
                        YYERROR;
                    }
                }
                ;

network_end:    TOK_PAR_CLOSE TOK_SEMI_COLON
                {
                    irc_config_network_t n = irc_config_last(config);

                    if (irc_config_network_host(n) == NULL ||
                        irc_config_network_port(n) == NULL) {
                        yyerror(scanner, config, "host and port must be set");
                        YYERROR;
                    }

                    if (irc_config_network_nick(n) == NULL) {
                        yyerror(scanner, config, "nick is not set");
                        YYERROR;
                    }
                }
                ;

network:        network_start network_options network_end
                ;

network_option: TOK_STRING TOK_EQUAL TOK_QUOTED_STRING TOK_SEMI_COLON
                {
                    irc_config_network_t n = irc_config_last(config);
                    char const *key = $1;
                    char const *value = strip_quote($3);

                    if (n == NULL) {
                        YYERROR;
                    }

                    if (strcmp(key, "host") == 0) {
                        irc_config_network_set_host(n, value);
                    } else if (strcmp(key, "port") == 0) {
                        irc_config_network_set_port(n, value);
                    } else if (strcmp(key, "nick") == 0) {
                        irc_config_network_set_nick(n, value);
                    } else if (strcmp(key, "nickserv") == 0) {
                        irc_config_network_set_nickserv(n, value);
                    } else if (strcmp(key, "nickserv_password") == 0) {
                        irc_config_network_set_nickserv_password(n, value);
                    } else if (strcmp(key, "ssl") == 0 ||
                               strcmp(key, "tls") == 0) {
                        bool val = (strcmp(value, "yes") == 0 ||
                                    strcmp(value, "true") == 0);
                        irc_config_network_set_ssl(n, val);
                    } else {
                        free($1);
                        free($3);

                        yyerror(scanner, config, "invalid option for network");
                        YYERROR;
                    }

                    free($1);
                    free($3);
                }
        ;

network_options:
        |       network_options network_option
        ;

comment:        TOK_COMMENT
        ;
%%
