#ifndef IRC_QUEUE_H
#define IRC_QUEUE_H

#include <irc/error.h>

typedef void (*free_t)(void*);

struct irc_queue;

typedef struct irc_queue *irc_queue_t;

irc_queue_t irc_queue_new(void);
void irc_queue_free(irc_queue_t q);

void irc_queue_clear(irc_queue_t q, free_t f);
irc_error_t irc_queue_push(irc_queue_t q, void *what);
void *irc_queue_pop(irc_queue_t q);

#endif
