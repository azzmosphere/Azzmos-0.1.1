SOURCES = CuTest.c CuTest.h \
	  $(top_srcdir)/lib/uriobj.c \
	  $(top_srcdir)/lib/utils.c \
	  $(top_srcdir)/lib/regexpr.c \
	  $(top_srcdir)/src/urinorm.c 
INCLUDES = @POSTGRESQL_CFLAGS@ -I$(top_srcdir)/include 
AM_CFLAGS = -I$(top_srcdir)/src
AM_LDFLAGS = @POSTGRESQL_LDFLAGS@ @LIBCURL@
test_uriobj_SOURCES = test_uriobj.c $(SOURCES) 
test_regexpr_SOURCES = test_regexpr.c $(SOURCES)
check_PROGRAMS = test_uriobj \
		 test_regexpr
TESTS =  test_uriobj \
	 test_regexpr
