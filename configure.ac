AC_INIT( [azzmos], [0.1.1], [azzmosphere@gmail.com])
AM_CONFIG_HEADER(config.h)

dnl Package information
AC_CONFIG_SRCDIR(bin/azzmos.c)
AM_INIT_AUTOMAKE

dnl program checks
AC_PROG_CC
AC_LANG_C
AM_PROG_CC_C_O
AC_PROG_LIBTOOL


AC_CONFIG_FILES([
  Makefile
  bin/Makefile
  test/Makefile
  include/Makefile
  lib/Makefile
])


dnl library checks
dnl AC_SEARCH_LIBS( pthread_mutex_lock, pthread, , AC_MSG_ERROR( [ libpthread is a required library] ), )
dnl AC_SEARCH_LIBS( strlen, c, , AC_MSG_ERROR( [strlen is a required function] ), )
LIBCURL_CHECK_CONFIG( , [ 7.10.03 ], , AC_MSG_ERROR( [ libCurl version 7.10.03 or above is required ] ))
AX_PATH_LIB_PCRE([], [AC_MSG_ERROR([pcre required to build])])

dnl The PG debugger should be required here,  so before updating 
dnl a PostgresQL version ensure the the pldebugger works with it.
AX_LIB_POSTGRESQL([8.2.6])
if test -z ${POSTGRESQL_CFLAGS}; then
	AC_MSG_ERROR([PostgreSQL >= 8.4.3 is required])
fi

dnl header file checks

dnl type checks

dnl structures checks 

dnl compiler character checks

dnl library function checks

dnl system services checks

dnl AC_SUBST(PACKAGE_STRING)

AC_OUTPUT
