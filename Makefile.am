AM_YFLAGS =		-d
BUILT_SOURCES =		lib/config_parse.h

lib_LTLIBRARIES =	libirc.la
libirc_la_SOURCES =	lib/irc.c \
			lib/client.c \
			lib/message.c \
			lib/queue.c \
			lib/strbuf.c \
			lib/pa.c \
			lib/util.c \
			lib/config.c \
			lib/config_parse.y \
			lib/config_lex.l

nobase_include_HEADERS = irc/error.h \
			 irc/irc.h \
			 irc/client.h \
			 irc/message.h \
			 irc/queue.h \
			 irc/pa.h \
			 irc/strbuf.h \
			 irc/util.h \
			 irc/config.h

AM_CFLAGS =		-I. -Wall -Werror

pkgconfig_DATA =	libirc.pc

SUBDIRS	=		. tests
