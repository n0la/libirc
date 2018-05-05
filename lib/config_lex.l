%option bison-bridge
%option reentrant

%{
#include <irc/config.h>
#include "config_parse.h"
%}

%%

network return TOK_NETWORK;

\{ return TOK_PAR_OPEN;
\} return TOK_PAR_CLOSE;
\= return TOK_EQUAL;
; return TOK_SEMI_COLON;

# {
    int c;

    while ((c = input(yyscanner)) != '\n' && c != EOF)
        ;

    if (c == EOF || c == '\n') {
        unput(c);
    }

    ++yylineno;

    return TOK_COMMENT;
}

[a-z\-\_]+ {
    yylval->string = strdup(yytext);
    return TOK_STRING;
}

\"(\\.|[^"\\])*\" {
    yylval->string = strdup(yytext);
    return TOK_QUOTED_STRING;
}

[ \t\n] ;

%%
