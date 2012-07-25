dnl Process this file with autoconf to produce a configure script.
AC_INIT(plugz, 0.0.1, jokillsya@gmail.com)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/main.c)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_FILES(Makefile src/Makefile)
AM_INIT_AUTOMAKE

AC_CONFIG_MACRO_DIR([m4])

LT_INIT
AC_ENABLE_STATIC
AC_ENABLE_SHARED
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
PKG_CHECK_MODULES(plugz, [glib-2.0 >= 2.3.0 gthread-2.0])
LIBS="$LIBS $plugz_LIBS"
AC_CHECK_HEADERS([locale.h stdlib.h])
AC_CHECK_FUNCS([setlocale])
AC_OUTPUT
