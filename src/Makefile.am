AM_LDFLAGS   = @LIBCURL@ \
	       @POSTGRESQL_LDFLAGS@ \
	       -L$(top_builddir)/lib -lazzmos

AM_CFLAGS = @POSTGRESQL_CFLAGS@ \
	    -I$(top_srcdir)/include

../lib/libazzmos.a:
	cd ../lib $(MAKE) $(AM_MAKEFLAGS)

bin_PROGRAMS = azzmos
azzmos_SOURCES = uriresolve.h uriresolve.c\
		 azzmos.c azzmos.h
