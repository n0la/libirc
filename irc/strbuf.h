#ifndef LIBIRC_STRBUF_H
#define LIBIRC_STRBUF_H

#include <stdlib.h>
#include <stdio.h>

struct strbuf_;

typedef struct strbuf_ *strbuf_t;

strbuf_t strbuf_new(void);
void strbuf_free(strbuf_t b);

ssize_t strbuf_append(strbuf_t b, char const *buf, ssize_t buflen);
ssize_t strbuf_getdelim(strbuf_t b, char **line, size_t *linesize, int delim);
ssize_t strbuf_getline(strbuf_t b, char **line, size_t *linesize);
ssize_t strbuf_getstr(strbuf_t b, char **line, size_t *linesize,
                      char const *small);
char *strbuf_strdup(strbuf_t b);

int strbuf_getc(strbuf_t b);
int strbuf_delete(strbuf_t b, int how);

void strbuf_reset(strbuf_t b);

size_t strbuf_len(strbuf_t b);

#endif
