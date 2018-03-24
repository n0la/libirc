#ifndef LIBIRC_ERROR_H
#define LIBIRC_ERROR_H

typedef enum {
    irc_error_success = 0,
    irc_error_argument,
    irc_error_memory,
    irc_error_internal,
    irc_error_protocol,
    irc_error_nodata,
    irc_error_state,
} irc_error_t;

#define IRC_SUCCESS(v) ((v) == irc_error_success)
#define IRC_FAILED(v)  (!IRC_SUCCESS(v))

#ifndef return_if_true
#define return_if_true(b, r) do {               \
        if ((b)) {                              \
            return r;                           \
        }                                       \
    } while (0)
#endif

#endif
