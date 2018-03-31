#include <irc/queue.h>

#include <stdlib.h>

struct irc_queue_item
{
    void *data;
    struct irc_queue_item *next;
    struct irc_queue_item *prev;
};

typedef struct irc_queue_item *irc_queue_item_t;

struct irc_queue
{
    irc_queue_item_t head;
    irc_queue_item_t tail;
};

irc_queue_t irc_queue_new(void)
{
    irc_queue_t q = NULL;

    q = calloc(1, sizeof(struct irc_queue));
    if (!q) {
        return NULL;
    }

    return q;
}

void irc_queue_free(irc_queue_t q)
{
    irc_queue_item_t it;

    if (!q) {
        return;
    }

    it = q->head;
    while (it != NULL) {
        irc_queue_item_t nx = it->next;
        free(it);
        it = nx;
    }

    free(q);
}

irc_error_t irc_queue_push(irc_queue_t q, void *what)
{
    irc_queue_item_t ny = NULL;

    return_if_true(q == NULL, irc_error_argument);

    ny = calloc(1, sizeof(struct irc_queue_item));
    return_if_true(ny == NULL, irc_error_memory);
    ny->data = what;

    if (q->head == NULL) {
        q->head = q->tail = ny;
    } else {
        ny->prev = q->tail;
        q->tail->next = ny;
        q->tail = ny;
    }

    return irc_error_success;
}

void *irc_queue_pop(irc_queue_t q)
{
    irc_queue_item_t it = NULL, prev = NULL;

    return_if_true(q == NULL, NULL);
    return_if_true(q->head == NULL, NULL);
    return_if_true(q->tail == NULL, NULL);

    it = q->tail;
    prev = it->prev;

    if (prev) {
        prev->next = NULL;
        q->tail = prev;
    } else {
        q->head = q->tail = NULL;
    }

    return it->data;
}

void irc_queue_clear(irc_queue_t q, free_t ff)
{
    void *p = NULL;

    while ((p = irc_queue_pop(q)) != NULL) {
        if (ff != NULL) {
            ff(p);
        }
    }
}
