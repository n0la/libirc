AM_YFLAGS =		-d

lib_LTLIBRARIES =	libirc.la
libirc_la_SOURCES =	lib/irc.c \
			lib/client.c \
			lib/message.c \
			lib/queue.c \
			lib/strbuf.c \
			lib/strv.c \
			lib/util.c

nobase_include_HEADERS = irc/error.h \
			 irc/irc.h \
			 irc/client.h \
			 irc/message.h \
			 irc/queue.h \
			 irc/strv.h \
			 irc/util.h

AM_CFLAGS =		-I. -Wall -Werror

pkgconfig_DATA =	libirc.pc

SUBDIRS	=		. tests
