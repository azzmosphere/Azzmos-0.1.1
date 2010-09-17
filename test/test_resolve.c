/*
 * =====================================================================================
 *
 *       Filename:  test_uriobj.c
 *
 *    Description:  integration test for resolution functions
 *
 *        Version:  1.0
 *        Created:  28/08/2010 12:03:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */
#include <CuTest.h>
#include <uriresolve.h>

void
test_uri_resolve_1( CuTest *tc )
{
	int gai_error = 0;
	uriobj_t uri;
	regexpr_t *re = (regexpr_t *) malloc( sizeof(regexpr_t));;
	char *href = strdup("http://www.example.com/");
		
	gai_error = uri_init_regex(re);
	if( ! gai_error ) {
		uri_parse(&uri, re, href);
		uri_normalize(&uri);
		gai_error = uri_resolve(&uri);
	}
	CuAssertIntEquals(tc, gai_error, 0);	
}

GetSuite()
{
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST( suite, test_uri_resolve_1);
}

int 
main()
{
	CuSuite  *suite  = CuSuiteNew();
	CuString *output = CuStringNew();
	CuSuiteAddSuite( suite, GetSuite());
	CuSuiteRun(suite);
	CuSuiteSummary( suite, output);
	fprintf( stdout, "%s\n", output->buffer);
	exit(suite->failCount);
}
