SOURCES = CuTest.c CuTest.h 
INCLUDES = @POSTGRESQL_CFLAGS@ -I$(top_srcdir)/include 
AM_CFLAGS = -I$(top_srcdir)/src
AM_LDFLAGS = @POSTGRESQL_LDFLAGS@ \
	     @LIBCURL@  \
	     -L$(top_builddir)/lib -lazzmos
test_uriobj_SOURCES = test_uriobj.c $(SOURCES) 
test_regexpr_SOURCES = test_regexpr.c $(SOURCES)
test_resolve_SOURCES = test_resolve.c \
			  $(SOURCES) \
			  $(top_srcdir)/src/uriresolve.c \
			  $(top_srcdir)/src/uriresolve.h 
check_PROGRAMS = test_uriobj \
		 test_regexpr \
		 test_resolve
TESTS =  test_uriobj \
	 test_regexpr
