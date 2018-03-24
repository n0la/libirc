AC_PREREQ([2.69])
AC_INIT([libirc], [0.1], [florian@n0la.org])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT()
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC

AC_PROG_LEX
AC_PROG_YACC

PKG_CHECK_MODULES([CMOCKA], [cmocka >= 1.0.1])

AC_CHECK_HEADER_STDBOOL()
AC_CONFIG_FILES([Makefile tests/Makefile])

PKG_INSTALLDIR
AC_CONFIG_FILES([libirc.pc])

AC_OUTPUT
