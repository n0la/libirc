AM_YFLAGS =		-d

lib_LTLIBRARIES =	libirc.la
libirc_la_SOURCES =	lib/irc.c \
			lib/message.c \
			lib/queue.c \
			lib/strbuf.c \
			lib/strv.c \
			lib/util.c

include_HEADERS =	lib/irc/error.h \
			lib/irc/irc.h \
			lib/irc/message.h \
			lib/irc/queue.h \
			lib/irc/strv.h \
			lib/irc/util.h

AM_CFLAGS =		-Ilib -Wall -Werror

pkgconfig_DATA =	libirc.pc

SUBDIRS	=		. tests
