#ifndef IRC_QUEUE_H
#define IRC_QUEUE_H

#include <irc/error.h>

struct irc_queue;

typedef struct irc_queue *irc_queue_t;

irc_queue_t irc_queue_new(void);
void irc_queue_free(irc_queue_t q);

irc_error_t irc_queue_push(irc_queue_t q, void *what);
void *irc_queue_pop(irc_queue_t q);

#endif
