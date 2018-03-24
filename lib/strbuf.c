#include <irc/strbuf.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct strbuf_
{
    char *buf;
    size_t bufsize;
    size_t end;
};

strbuf_t strbuf_new(void)
{
    strbuf_t m = NULL;

    m = calloc(1, sizeof(struct strbuf_));
    if (!m) {
        return NULL;
    }

    return m;
}

void strbuf_free(strbuf_t b)
{
    if (b == NULL) {
        return;
    }

    free(b);
}

void strbuf_reset(strbuf_t b)
{
    if (b == NULL) {
        return;
    }

    free(b->buf);
    b->buf = NULL;

    b->end = b->bufsize = 0;
}

ssize_t strbuf_append(strbuf_t b, char const *buffer, ssize_t len)
{
    if (b == NULL || buffer == NULL) {
        return -1;
    }

    if (len < 0) {
        len = strlen(buffer);
    }

    if ((len+b->end) > b->bufsize) {
        char *tmp = NULL;

        tmp = recallocarray(b->buf, b->bufsize+1,
                            b->bufsize+len+1,
                            sizeof(char)
            );
        if (tmp == NULL) {
            return -2;
        }

        b->buf = tmp;
        b->bufsize += len;
    }

    memcpy(b->buf + b->end, buffer, len);
    b->end += len;

    if (b->end < b->bufsize) {
        memset(b->buf+b->end, 0, b->bufsize - b->end);
    }

    return len;
}

ssize_t strbuf_getline(strbuf_t b, char **line, size_t *linesize)
{
    return strbuf_getdelim(b, line, linesize, '\n');
}

ssize_t strbuf_getdelim(strbuf_t b, char **line, size_t *linesize, int delim)
{
    char *pos = NULL;
    ssize_t diff = 0;
    char *l = NULL;

    if (b->end == 0) {
        return -1;
    }

    pos = memchr(b->buf, delim, b->end);
    if (pos == NULL) {
        /* not found, so entire string
         */
        pos = b->buf + b->end - 1;
    }

    diff = (ssize_t)(pos - b->buf) + 1;
    l = calloc(diff + 1, sizeof(char));
    if (l == NULL) {
        return -2;
    }

    memcpy(l, b->buf, diff);

    memmove(b->buf, pos+1, b->end - diff);
    b->end -= diff;

    memset(b->buf + b->end, 0, b->bufsize - b->end);

    *line = l;
    *linesize = diff;

    return 0;
}

ssize_t strbuf_getstr(strbuf_t b, char **line, size_t *linesize,
                      char const *small)
{
    char *pos = NULL;
    size_t diff = 0;
    size_t slen = strlen(small);
    char *l = NULL;

    if (b->end == 0) {
        return -1;
    }

    pos = strstr(b->buf, small);
    if (pos == NULL) {
        return -1;
    }

    diff = ((size_t)(pos - b->buf)) + slen;
    l = calloc(diff + 1, sizeof(char));
    if (l == NULL) {
        return -2;
    }

    memcpy(l, b->buf, diff);

    b->end -= diff;
    memmove(b->buf, pos+slen, b->end);
    memset(b->buf + b->end, 0, b->bufsize - b->end);

    *line = l;
    *linesize = diff;

    return 0;
}

char *strbuf_strdup(strbuf_t b)
{
    if (b == NULL || b->buf == NULL) {
        return NULL;
    }

    return strdup(b->buf);
}

size_t strbuf_len(strbuf_t b)
{
    if (b == NULL) {
        return 0;
    }

    return b->end;
}

int strbuf_getc(strbuf_t b)
{
    if (b == NULL || b->end <= 0) {
        return -1;
    }

    return *b->buf;
}

int strbuf_delete(strbuf_t b, int how)
{
    if (b == NULL || how > b->end) {
        return -1;
    }

    b->end -= how;
    memmove(b->buf, b->buf+how, b->end);
    memset(b->buf + b->end, 0, b->bufsize - b->end);

    return 0;
}
