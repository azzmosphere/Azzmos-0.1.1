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
#include <azzmos/uriobj.h>

regexpr_t *re;

void
test_uri_remove_dot_segments_1(CuTest *tc)
{
	char *path = strdup("/a/b/c/./../../g");
	CuAssertStrEquals(tc, "/a/g",  uri_remove_dot_segments(&path));
}

void
test_uri_remove_dot_segments_2(CuTest *tc)
{
	char *path = strdup("mid/content=5/../6");
	CuAssertStrEquals(tc, "mid/6",  uri_remove_dot_segments(&path));
}

void
test_uri_remove_dot_segments_3(CuTest *tc)
{
	char *path = strdup("../../mid/6");
	CuAssertStrEquals(tc, "/mid/6",  uri_remove_dot_segments(&path));
}

void
test_uri_parse1(CuTest *tc)
{
	int err = 0;
	re = (regexpr_t *) malloc( sizeof(regexpr_t));	
	err = uri_init_regex(re); 
	CuAssertIntEquals(tc, 0, err);
}

void
test_uri_parse2(CuTest *tc)
{
	char *fqp = "http://www.ics.uci.edu/pub/ietf/uri/#Related";
	char *errptr;
	int erroffset;
	int ovector[60];
	uriobj_t uri;
	int   err = err = uri_parse( &uri, re, fqp);
	CuAssertIntEquals(tc, 0, err);
}

void
test_uri_parse3(CuTest *tc)
{
	char *fqp = "http://www.ics.uci.edu/pub/ietf/uri/#Related";
	char *errptr;
	int erroffset;
	int ovector[60];
	uriobj_t uri;
	int   err  = uri_parse( &uri, re, fqp);
	if( err ) {
		CuAssertIntEquals(tc,0,1);
		return;
	}
	err  = strcmp("http", *(uri.uri_scheme));
	err += strcmp("www.ics.uci.edu", *(uri.uri_auth));
	err += strcmp("/pub/ietf/uri/", *(uri.uri_path));
	err += strcmp("Related", *(uri.uri_frag));
	CuAssertIntEquals(tc,0,err);
}

void
test_uri_merge_paths1(CuTest *tc)
{
	char *bpath  = strdup("http://www.example.com/a/path/to/uri"),
	     *rpath  = strdup("path/to/uri"),
	     *expect = strdup("/a/path/to/path/to/uri");
	uriobj_t base,
		 rel;
	uri_parse(&base,re, bpath);
	uri_parse(&rel, re, rpath);
	CuAssertStrEquals(tc, uri_merge_paths(&base,&rel), expect);
}

void
test_uri_merge_paths2(CuTest *tc)
{
	char *bpath = strdup("http://www.example.com"),
	     *rpath = strdup("path/to/uri"),
	     *expect = strdup("/path/to/uri");
	uriobj_t base,
		 rel;
	uri_parse(&base, re, bpath);
	uri_parse(&rel, re, rpath);
	CuAssertStrEquals(tc, uri_merge_paths(&base,&rel),expect);
}

void
test_uri_trans_ref1(CuTest *tc)
{
	char *bpath = strdup("http://www.example.com"),
	     *rpath = strdup("../path/to/uri.html");
	uriobj_t base,
		 rel,
		*tran;
	int cmp = 0;
	uri_parse(&base, re, bpath);
	uri_parse(&rel, re, rpath);
	tran = uri_trans_ref(&base, &rel, true); 
	cmp  = strcmp("http", *tran->uri_scheme);
	cmp += strcmp("www.example.com",*tran->uri_auth);
	cmp += strcmp("/path/to/uri.html",*tran->uri_path);
	CuAssertIntEquals(tc,cmp,0);
}

void
test_uri_trans_ref2(CuTest *tc)
{
	char *bpath = strdup("http://www.example.com"),
	     *rpath = strdup("http://www.new.com/../path/to/uri.html");
	uriobj_t base,
		 rel,
		*tran;
	int cmp = 0;
	uri_parse(&base, re, bpath);
	uri_parse(&rel, re, rpath);
	tran = uri_trans_ref(&base, &rel, true); 
	cmp  = strcmp("http", *tran->uri_scheme);
	cmp += strcmp("www.new.com",*tran->uri_auth);
	cmp += strcmp("/path/to/uri.html",*tran->uri_path);
	CuAssertIntEquals(tc,cmp,0);
}

void
test_uri_trans_ref3(CuTest *tc)
{
	char *bpath = strdup("http://www.example.com"),
	     *rpath = strdup("../path/to/uri.html?query&e=b");
	uriobj_t base,
		 rel,
		*tran;
	int cmp = 0;
	uri_parse(&base, re, bpath);
	uri_parse(&rel, re, rpath);
	tran = uri_trans_ref(&base, &rel, true); 
	cmp  = strcmp("http", *tran->uri_scheme);
	cmp += strcmp("www.example.com",*tran->uri_auth);
	cmp += strcmp("/path/to/uri.html",*tran->uri_path);
	cmp += strcmp("query&e=b",*tran->uri_query);
	CuAssertIntEquals(tc,cmp,0);
}

void
test_uri_trans_ref4(CuTest *tc)
{
	char *bpath = strdup("http://www.example.com/old/path"),
	     *rpath = strdup("/new/path/uri.html?query&e=b");
	uriobj_t base,
		 rel,
		*tran;
	int cmp = 0;
	uri_parse(&base, re, bpath);
	uri_parse(&rel, re, rpath);
	tran = uri_trans_ref(&base, &rel, true); 
	cmp  = strcmp("http", *tran->uri_scheme);
	cmp += strcmp("www.example.com",*tran->uri_auth);
	cmp += strcmp("/new/path/uri.html",*tran->uri_path);
	cmp += strcmp("query&e=b",*tran->uri_query);
	CuAssertIntEquals(tc,cmp,0);
}

void
test_uri_trans_ref5(CuTest *tc)
{
	char *bpath = strdup("http://www.example.com/this/is/a/buf/old/path"),
	     *rpath = strdup("../../new/path/uri.html");
	uriobj_t base,
		 rel,
		*tran;
	int cmp = 0;
	uri_parse(&base, re, bpath);
	uri_parse(&rel, re, rpath);
	tran = uri_trans_ref(&base, &rel, true); 
	cmp  = strcmp("http", *tran->uri_scheme);
	cmp += strcmp("www.example.com",*tran->uri_auth);
	cmp += strcmp("/this/is/a/new/path/uri.html",*tran->uri_path);
	CuAssertIntEquals(tc,cmp,0);
}

void
test_uri_trans_ref6(CuTest *tc)
{
	char *bpath = strdup("http://www.example.com/this/is/a/buf/old/path#fragment"),
	     *rpath = strdup("../../new/path/uri.html");
	uriobj_t base,
		 rel,
		*tran;
	int cmp = 0;
	uri_parse(&base, re, bpath);
	uri_parse(&rel, re, rpath);
	tran = uri_trans_ref(&base, &rel, true); 
	cmp  = strcmp("http", *tran->uri_scheme);
	cmp += strcmp("www.example.com",*tran->uri_auth);
	cmp += strcmp("/this/is/a/new/path/uri.html",*tran->uri_path);
	CuAssertIntEquals(tc,cmp,0);
}

void
test_uri_trans_ref7(CuTest *tc)
{
	char *bpath = strdup("http://www.example.com/this/is/a/buf/old/path#fragment"),
	     *rpath = strdup("www.example.com/../../new/path/uri.html");
	uriobj_t base,
		 rel,
		*tran;
	int cmp = 0;
	uri_parse(&base, re, bpath);
	uri_parse(&rel, re, rpath);
	tran = uri_trans_ref(&base, &rel, true); 
	cmp  = strcmp("http", *tran->uri_scheme);
	cmp += strcmp("www.example.com",*tran->uri_auth);
	cmp += strcmp("/this/is/a/buf/new/path/uri.html",*tran->uri_path);
	CuAssertIntEquals(tc,cmp,0);
}

test_uri_comp_recomp_1(CuTest *tc)
{
	char *expect = strdup("http://www.example.com/test/func.cgi?x=y&z=j");
	uriobj_t uri;
	uri_parse(&uri, re, expect);
	CuAssertStrEquals(tc, expect, uri_comp_recomp( &uri ));
}

CuSuite *
GetSuite()
{
	CuSuite *suite = CuSuiteNew();
	SUITE_ADD_TEST( suite, test_uri_remove_dot_segments_1);
	SUITE_ADD_TEST( suite, test_uri_remove_dot_segments_2);
	SUITE_ADD_TEST( suite, test_uri_remove_dot_segments_3);
	SUITE_ADD_TEST( suite, test_uri_parse1);
	SUITE_ADD_TEST( suite, test_uri_parse2);
	SUITE_ADD_TEST( suite, test_uri_parse3);
	SUITE_ADD_TEST( suite, test_uri_merge_paths1);
	SUITE_ADD_TEST( suite, test_uri_merge_paths2);
	SUITE_ADD_TEST( suite, test_uri_trans_ref1);
	SUITE_ADD_TEST( suite, test_uri_trans_ref2);
	SUITE_ADD_TEST( suite, test_uri_trans_ref3);
	SUITE_ADD_TEST( suite, test_uri_trans_ref4);
	SUITE_ADD_TEST( suite, test_uri_trans_ref5);
	SUITE_ADD_TEST( suite, test_uri_trans_ref6);
	SUITE_ADD_TEST( suite, test_uri_trans_ref7);
	SUITE_ADD_TEST( suite, test_uri_comp_recomp_1);
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


