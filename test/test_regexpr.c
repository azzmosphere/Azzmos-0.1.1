/*
 * =====================================================================================
 *
 *       Filename:  test_uriobj.c
 *
 *    Description:  tests the algoritms of uriobj.c
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
#include <azzmos/regexpr.h>

#define ID 0
#define RE "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?"

void 
test_regexpr( CuTest *tc)
{
	int err = 0;
	regexpr_t *re;
	re = (regexpr_t *) malloc( sizeof(regexpr_t));
	err = re_init( re, ID);
	err = re_comp( re, ID, RE, 0,NULL); 
	CuAssertIntEquals( tc, err, 0);
}

void
test_regexpr_exec( CuTest *tc)
{
	int err,
	    id = ID;
	regexpr_t *re;
	re = (regexpr_t *) malloc( sizeof(regexpr_t));
	re_init(re, id);
	err = re_comp( re, id, RE, 0, NULL);
	if( err == 0 ) {
		*(re->re_subject) = strdup("http://www.ics.uci.edu/pub/ietf/uri/#Related");
		re->re_length = strlen(*(re->re_subject));
		err = re_exec(re, ID);
	}
	if( err >= 0 ) {
		err = 0;
	}
	CuAssertIntEquals( tc, err, 0);
}

CuSuite *
GetSuite()
{
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST( suite, test_regexpr);
	SUITE_ADD_TEST( suite, test_regexpr_exec);
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


