INCLUDES = -I$(top_srcdir)/include \
	   @POSTGRESQL_CFLAGS@ 
lib_LTLIBRARIES = libazzmos.la
libazzmos_la_SOURCES = uriobj.c \
		       utils.c \
		       regexpr.c \
		       urinorm.c
AM_LDFLAGS = @POSTGRESQL_LDFLAGS@ \
	     @LIBCURL@

