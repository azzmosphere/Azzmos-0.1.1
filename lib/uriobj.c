/*
 * =====================================================================================
 *
 *       Filename:  uriobj.c
 *
 *    Description:  Implements RFC3986 using the uriobj_t object to define
 *                  a URI.
 *
 *        Version:  1.0
 *        Created:  27/08/2010 22:28:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#include <azzmos/uriobj.h>


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */
#define RE_ID 0
#define RE    "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?"

/************************************************************************************** 
 * All though %7E and ~ are equivalants for the purpose of URI uniquness tilda's are 
 * also transposed to the '~' character.  This is done after percentage encoding 
 * checking is performed.
 **************************************************************************************/
#define RE_TILDA_ID 1
//#define RE_REPLACE_TILDA "(%7E)"

/**************************************************************************************
 * The path section is aloud to have reserved charcters embedded as percentage encoded
 * digits.  These have to be checked to see if they are not a dangerous character.
 **************************************************************************************/
#define RE_PC_ID 2
//#define RE_PERCENT_ENC   "(%??)"

/**************************************************************************************
 * The following functions are stop the program from breaking by trying to malloc NULL
 **************************************************************************************/
#define URI_CP_PT(p) (*p)?strdup(*p):NULL
#define UI(u) ((*u)?*u:"")

/**************************************************************************************
 * PCRE has the following equivilants for PERL subqueries,  that is $1 = ovector[2] and
 * ovector[3]. Below is a description of how these map to RFC3986 Appendix B regular 
 * expression which is defined by the macro RE.
 *    $0 = 0,1
 *    $1 = 2,3
 *    $2 = 4,5
 *    $3 = 6,7
 *    $4 = 8,9
 *    $5 = 10,11
 *    $6 = 12,13
 *    $7 = 14,15
 *    $8 = 16,17
 *    $9 = 18,19
 **************************************************************************************/
#define RE_S_S 0x04
#define RE_S_E 0x05
#define RE_A_S 0x08
#define RE_A_E 0x09
#define RE_P_S 0x0a
#define RE_P_E 0x0b
#define RE_Q_S 0x0e 
#define RE_Q_E 0x0f
#define RE_F_S 0x12
#define RE_F_E 0x13


/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
static char *pop_segment( char **path);
static char *shift_segment( char **path, const int offset);
static char *get_next_segment( char **path);
static char *replace_prefix( char **path);
static void  init_uriobj_str( uriobj_t *uri);
inline static char *uri_strcpy( char **s1, const char *s2);
inline static char *uri_strcat( char *s1, const char *format, const char *s2);
static bool is_gen_delims( char c);
static bool is_sub_delim( char c );
static bool is_reserved( char c );
static bool is_unreserved( char c );
static bool is_pct_encoded( char *s );
static bool is_scheme_char( char c );

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_remove_dot_segments
 *  Description:  Implements RFC 3986 section 5.2.4 
 *                use for interpreting and removing the special "." and ".." complete 
 *                path segments from a referenced path.  This is done after the path is
 *                extracted from a reference, whether or not the path was relative, in
 *                order to remove any invalid or extraneous dot-segments prior to
 *                forming the target URI.  Although there are many ways to accomplish
 *                this removal process, we describe a simple method using two string
 *                buffers.
 *                
 * =====================================================================================
 */
extern char *
uri_remove_dot_segments( char **path )
{
	char *ou_buffer = NULL,
	     *in_buffer = NULL,
	     *segment   = NULL,
	     sl = '\0';
	int len = 0;
	if( *(path) == NULL ) {
		return NULL;
	}
	in_buffer = strdup( *(path));
	len = strlen( in_buffer ) + 1;
	ou_buffer = (char *) malloc( len );
	ou_buffer[0] = '\0';
	while( in_buffer != NULL ){
		segment = get_next_segment( &in_buffer);
		if( strcmp( segment, "../") == 0 || strcmp(segment,"./") == 0){
			shift_segment( &in_buffer, 0);
		}
		else if( strcmp(segment, "/./") == 0 || strcmp(segment,"/.") == 0){
			replace_prefix( &in_buffer);
		}
		else if( strcmp( segment,"/../") == 0 || strcmp(segment,"/..") == 0){
			replace_prefix(&in_buffer);
			pop_segment(&ou_buffer);
		}
		else if( strcmp( in_buffer, ".") == 0 || strcmp( in_buffer, "..") == 0){
			in_buffer = NULL;
		}
		else if( segment) {
			sl = segment[ strlen(segment) - 1];	
			if( sl == '/'){
				segment = shift_segment(&in_buffer, 0);
			}
			else {
				segment = shift_segment(&in_buffer, 1);
			}
			strcat(ou_buffer, segment);
		}
	}
	len = strlen(ou_buffer) + 1;
	realloc( (void *) ou_buffer, len);
	return ou_buffer;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_init_regex
 *  Description:  Initilises the regular expresion that is used break up the FQP into a 
 *                uri object.  This function must be the first function called by the
 *                scheduler when it is initlizing the regular expersion object.
 * =====================================================================================
 */
extern int    
uri_init_regex( regexpr_t *re)
{
	int err = 0;
	err = re_init(re, RE_ID);
	if( err == 0 ) {
		err = re_comp( re, RE_ID, RE, 0, NULL);
	}
	return err;

}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_trans_ref
 *  Description:  Attempt to implement algorithm 5.2.2 of RFC3986, For each URI reference
 *               (R), transform R into its target URI (T). The return value of this 
 *               routine is (T).
 * =====================================================================================
 */
extern uriobj_t *
uri_trans_ref( uriobj_t *base, uriobj_t *ref, bool strict)
{
	uriobj_t *trans = (uriobj_t *) malloc(sizeof(uriobj_t));
	if( errno ) {
		return NULL;
	}
	init_uriobj_str(trans);
	if( ! strict && (strcmp(UI(base->uri_scheme), UI(ref->uri_scheme)) == 0)){
		*(ref->uri_scheme) = NULL;
	}
	if( *(ref->uri_scheme)){
		*(trans->uri_scheme) = URI_CP_PT(ref->uri_scheme);
		*(trans->uri_auth)   = URI_CP_PT(ref->uri_auth);
		*(trans->uri_path)   = uri_remove_dot_segments( ref->uri_path );
		*(trans->uri_query)  = URI_CP_PT(ref->uri_query);
	}
	else {
		if( *(ref->uri_auth) ){
			*(trans->uri_auth) = URI_CP_PT(ref->uri_auth);
			*(trans->uri_path) = uri_remove_dot_segments( ref->uri_path );
			*(trans->uri_query) = URI_CP_PT(ref->uri_query);
		}
		else{
			if( ! ref->uri_path ){
				*(trans->uri_path)   = URI_CP_PT(base->uri_path);
				if( *ref->uri_query){
					*(trans->uri_query) = URI_CP_PT(ref->uri_query);
				}
				else {
					*(trans->uri_query) = URI_CP_PT(base->uri_query);
				}
			}
			else {
				if( *(ref->uri_path)[0] == '/'){
					*(trans->uri_path) = uri_remove_dot_segments(ref->uri_path);
				}
				else{
					*(trans->uri_path) = uri_merge_paths(base, ref);
					*(trans->uri_path) = uri_remove_dot_segments(trans->uri_path);
				}
				*(trans->uri_query) = URI_CP_PT(ref->uri_query);
			}
			uri_strcpy(trans->uri_auth, *base->uri_auth);
		}
		uri_strcpy(trans->uri_scheme, *base->uri_scheme);
	}
	*(trans->uri_frag) = URI_CP_PT(ref->uri_frag);
	return trans;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_parse
 *  Description:  URI objects can be created in two ways: 
 *                1) By downloading the uri from the database; or
 *                2) By creating the URI using a FQP string.
 *
 *                The uri_parse functin does not assume that the URI exists before it
 *                creates the object.  When it is call the object is created without an
 *                id.  This will need to be allocated by uri_db_update and should be done
 *                after the path is resolved.
 *
 *                On success the uri object will have the members returned by the re 
 *                allocated and zero will be returned.  On failure it will return a URI 
 *                offset error.
 * =====================================================================================
 */
extern int
uri_parse( uriobj_t *uri, regexpr_t *re, const char *fqp)
{
	int err = 0;
	init_uriobj_str(uri);
	if( errno ) {
		return errno;
	}
	*(re->re_subject) = strdup(fqp);
	re->re_length = strlen(fqp);
	err = re_exec( re, RE_ID);
	if( err > 0 ) {
		err = 0;
		*(uri->uri_scheme) = usplice(fqp, re->re_ovector[RE_S_S], re->re_ovector[RE_S_E] -1);
		*(uri->uri_auth)   = usplice(fqp, re->re_ovector[RE_A_S], re->re_ovector[RE_A_E] -1);
		*(uri->uri_path)   = usplice(fqp, re->re_ovector[RE_P_S], re->re_ovector[RE_P_E] -1);
		*(uri->uri_query)  = usplice(fqp, re->re_ovector[RE_Q_S], re->re_ovector[RE_Q_E] -1);
		*(uri->uri_frag)   = usplice(fqp, re->re_ovector[RE_F_S], re->re_ovector[RE_F_E] -1);
		uri->uri_id = uri->uri_flags = 0;
		*(uri->uri_host) = *(uri->uri_port)
			         = *(uri->uri_ip)
			         = NULL;
	}
	return err;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_merge_paths
 *  Description:  This a attempt at implementing RFC3986 section 5.2.3. 
 *
 *                If the base URI has a defined authority component and an empty 
 *                path, then return a string consisting of "/" concatenated with the 
 *                reference's path; otherwise, 
 *
 *                return a string consisting of the reference's path component 
 *                appended to all but the last segment of the base URI's path (i.e., 
 *                excluding any characters after the right-most "/" in the base URI 
 *                path, or excluding the entire base URI path if it does not contain 
 *                any "/" characters).
 *  
 * =====================================================================================
 */
extern char *
uri_merge_paths( uriobj_t *base, uriobj_t *rel)
{
	char *path,
	     *bpath = *(base->uri_path),
	     *rpath = *(rel->uri_path);
	int   len = 0;
	if( base->uri_auth && !(bpath) ) {
		if( !(rpath)){
			path = strdup("/");
		}	
		else {
			len = strlen(rpath) + 2;
			path = (char *) malloc(len * sizeof(char));
			path[0] = '/';
			path[1] = '\0';
			strncat(path,rpath,BUFSIZ);
		}
	}
	else {
		pop_segment(&bpath);
		len = strlen(rpath) + strlen(bpath) + 1;
		path = (char *) malloc(len * sizeof(char));
		strncat(path, bpath, BUFSIZ);
		strncat(path, strdup("/"), BUFSIZ);
		strncat(path, rpath, BUFSIZ); 
	}
	return path;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_comp_recomp
 *  Description: Parsed URI components can be recomposed to obtain the corresponding 
 *               URI reference string. This is a attempt at implementing RFC3986 section
 *               5.3.
 * =====================================================================================
 */
extern char *
uri_comp_recomp( uriobj_t *uri)
{
	char *result;
	int   len = strlen(*uri->uri_path);
	if( *uri->uri_scheme ) {
		asprintf(&result, "%s:",*uri->uri_scheme);
	}
	if( *uri->uri_auth){
		result = uri_strcat(result,"%s//%s", *uri->uri_auth);
	}
	result = uri_strcat(result,"%s%s",*uri->uri_path);
	if( *uri->uri_query ){
		result = uri_strcat(result,"%s?%s",*uri->uri_query);
	}
	if( *uri->uri_frag ) {
		result = uri_strcat(result,"%s#%s",*uri->uri_frag);
	}
	return result;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_norm_scheme
 *  Description:  Normalize the scheme section of the uri. Return 0 on success or 
 *                EILSEQ or ENOATTR 
 * =====================================================================================
 */
extern int
uri_norm_scheme( uriobj_t *uri) 
{
	int rv = 0,
	    len,
	    i;
	char *scheme;
	if( ! *uri->uri_scheme || !strlen(*uri->uri_scheme)) {
		rv = ENOATTR;
	}
	else if( ! isalpha(*uri->uri_scheme[0])){
		rv = EILSEQ;
	}
	else {
		scheme = strdup( *uri->uri_scheme);
		len    = strlen(scheme);
		for(i = 0; i < len;i ++) {
			if( ! is_scheme_char(scheme[i])){
				rv = EILSEQ;
				break;
			}
			if( isalpha(scheme[i]) && isupper(scheme[i])){
				scheme[i] = tolower(scheme[i]);
			}
		}
	}
	if( rv == 0 ){
		*(uri->uri_scheme) = NULL;
		free(*uri->uri_scheme);
		*(uri->uri_scheme) = scheme;
	}
	return rv;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  norm_pct
 *  Description:  Normailize a pct-encoded character in accordance to section 2.1 of RFC
 *                3986. 
 * =====================================================================================
 */
extern int
norm_pct( char **pct)
{
	char *p = strdup(*pct);
	int err = 0;
 	if(!is_pct_encoded(p)){
		err = EILSEQ;
	}
	if( err == 0 ) {
		if( isalpha(p[1]) && islower(p[1])){
			p[1] = toupper(p[1]);
		}
		if( isalpha(p[2]) && islower(p[2])){
			p[2] = toupper(p[2]);
		}
	}
	*(pct) = NULL;
	free(*pct);
	*(pct) = strdup(p);
	return err;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_norm_host
 *  Description:  Normalize the host section of the URI if it is set.  The host section
 *                that is normalized here is the reg-name, and not the IP address.
 *                The IP address should be inside the IP section of the structure.
 *
 *                This section attempts to implement section 3.2.2 of RFC3986 with the
 *                exception that userinfo is not checked as HTTP and HTTPS do not 
 *                specify the usage of it.
 * =====================================================================================
 */
extern int
uri_norm_host( uriobj_t *uri)
{
	int   err  = 0, i, len;
	char *pct  = (char *) malloc(4),
	     *host = URI_CP_PT(uri->uri_host);
	if( host ) {
		len = strlen(host);
		for(i = 0;i < len; i ++ ) {
			if( host[i] == '%'){
				if( (i + 2) > len ) {
					err = EILSEQ;
					break;
				}
				pct[0] = host[i ++];
				pct[1] = host[i ++];
				pct[2] = host[i ++];
				pct[3] = '\0';
				err = norm_pct(&pct);
				if( err ) {
					break;
				}
				host[(i - 3)] = pct[0];
				host[(i - 2)] = pct[1];
				host[(i - 1)] = pct[2];
				continue;
			}
			if(!(is_sub_delim(host[i]) || is_unreserved(host[i]))){
				err = EILSEQ;
				break;	
			}
			if( isalpha(host[i]) && isupper(host[i]) ){
				host[i] = tolower(host[i]);
			}
		}
		if( ! err ) {
			*(uri->uri_host) = NULL;
			free(*uri->uri_host);
			*(uri->uri_host) = host;
		}
	}
	return err;
}

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  pop_segment
 *  Description: get the last segment in the path not including
 *               the initial "/" character (if any) and any subsequent characters up to, 
 *               but an including, the last "/"
 *
 * =====================================================================================
 */
static char *
pop_segment( char **path)
{
	int i = 0,
	    s = 0,
	    len = 0;
	char *segment,
	     *pref = URI_CP_PT(path);
	if( !pref || !strlen(pref)){
		return NULL;
	}
	len = i = strlen(pref);
	for(; i > 0; i --){
		if( pref[i] == '/'){
			break;
		}

	}
	*(path) = usplice(pref, 0, (i -1));
	segment = (char *) malloc( len + 1);
	for(; i < len; i ++){
		segment[s ++] = pref[i];
	}
	segment[s] = '\0';
	realloc( (void *) segment, s);
	return segment;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  pop_segment
 *  Description: get the first path segment from the path, including the initial 
 *               "/" character (if any) and any subsequent characters up to, 
 *               but not including, the next "/", reset
 *
 * =====================================================================================
 */
static char *
shift_segment( char **path, const int offset)
{
	int  len = 0,
	     i = 0;
	char *segment,
	     *pref = URI_CP_PT(path);
	if( ! pref  || ! strlen(pref) ){
		return NULL;
	}
	len = strlen( pref );
	segment = (char *) malloc( len + 1);
	segment[0] = pref[0]; 
	for( i = 1; i < len; i ++){
		if( pref[i] == '/' ) {
			break;
		}
		segment[i] = pref[i];
	}
	*(path) = usplice( pref, (i + offset), (unsigned int) NULL);
	segment[++i] = '\0';
	realloc( (void *) segment, i);
	return segment;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  get_next_segment
 *  Description:  return the next segment from the path
 * =====================================================================================
 */
static char *
get_next_segment( char **path)
{
	char *pref,
	     *segment;
	int i = 0,
	    len = 0;
	if( *(path) == NULL || !strlen(*(path))){
		return NULL;
	}
	pref = strdup( *(path));
	len  = strlen( pref );
	segment = (char *) malloc( strlen(pref));
	segment[i] = pref[i];
	if( strlen(pref) > 1 ) {
		for( i = 1; i < len; i ++){
			segment[i] = pref[i];	
			if( segment[i] == '/'){
				break;
			}
		}
	}
	segment[ ++i] = '\0';
	realloc( (void *) segment, i);
	return segment;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  replace_prefix
 *  Description:  replace the prefix segment of path with '/'
 * =====================================================================================
 */
static char *
replace_prefix( char **path)
{
	char  *pref,
	      *segment,
	      *tmp;
	if( *(path) == NULL || !strlen(*(path))){
		return NULL;
	}
	pref = strdup( *(path));
	segment = shift_segment( &pref, 1);
	if( pref == NULL){
		tmp = strdup("/");
	}
	else{
		tmp = (char *) malloc( strlen(pref) + 2);
		tmp[0] = '/';
		tmp[1] = '\0';
		strcat(tmp, pref);
	}
	*(path) = tmp;
	return segment;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  init_uriobj_str
 *  Description:  Initilize the string pointer components of the uri object. 
 * =====================================================================================
 */
static void
init_uriobj_str( uriobj_t *uri)
{
	uri->uri_scheme = (char **) malloc(sizeof(char *));
	uri->uri_auth   = (char **) malloc(sizeof(char *));
	uri->uri_path   = (char **) malloc(sizeof(char *));
	uri->uri_query  = (char **) malloc(sizeof(char *));
	uri->uri_frag   = (char **) malloc(sizeof(char *));
	uri->uri_host   = (char **) malloc(sizeof(char *));
	uri->uri_port   = (char **) malloc(sizeof(char *));
	uri->uri_ip     = (char **) malloc(sizeof(char *));
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_strcpy
 *  Description:  Copy s2 to s1.
 * =====================================================================================
 */
inline static char *
uri_strcpy( char **s1, const char *s2)
{
	if( s2 == NULL) {
		*(s1) = NULL;
		return NULL;
	}
	asprintf(s1, "%s", s2);
	return *s1;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_strcat
 *  Description:  Concadinate s2 to s1 using a format string.  The format string should
 *                contain two '%s'.  This is a wrapper function to stop memory leaks.
 * =====================================================================================
 */
inline static char *
uri_strcat( char *s1, const char *format, const char *s2)
{
	char *buf;
	asprintf(&buf,format,s1,s2);
	s1 = NULL;
	free(s1);
	s1 = buf;
	return s1;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  is_gen_delim
 *  Description:  Tests a char to see if it a gen-delim character, returns true for
 *                success or false on failure.
 * =====================================================================================
 */
static bool
is_gen_delims( char c)
{
	bool rv = false;
	switch( c ) {
		case(':'):
			rv = true;
			break;
		case('/'):
			rv = true;
			break;
		case('?'):
			rv = true;
			break;
		case('#'):
			rv = true;
			break;
		case('['):
			rv = true;
			break;
		case(']'):
			rv = true;
			break;
		case('@'):
			rv = true;
	}
	return rv;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  is_sub_delim
 *  Description:  Checks to see if a character is a sub-delim character.
 * =====================================================================================
 */
static bool
is_sub_delim( char c )
{
	bool rv = false;
	switch( c ){
		case('!'):
			rv = true;
			break;
		case('$'):
			rv = true;
			break;
		case('&'):
			rv = true;
			break;
		case('\''):
			rv = true;
			break;
		case('('):
			rv = true;
			break;
		case(')'):
			rv = true;
			break;
		case('*'):
			rv = true;
			break;
		case('+'):
			rv = true;
			break;
		case(','):
			rv = true;
			break;
		case(';'):
			rv = true;
			break;
		case('='):
			rv = true;
	}
	return rv;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  is_reserved
 *  Description:  Reserved characters can only be used in certain parts of the URI
 *                string.
 * =====================================================================================
 */
static bool
is_reserved( char c )
{
	bool rv = false;
	if( is_gen_delim(c) || is_sub_delim(c) ) {
		rv = true;
	}
	return rv;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  is_unreserved
 *  Description:  A unreserved character can appear in any part of the URI 
 * =====================================================================================
 */
static bool
is_unreserved( char c )
{
	bool rv = false;
	if( isalpha(c) ){
		rv = true;
	}
	else if( isdigit(c)){
		rv = true;
	}
	else {
		switch(c){
			case('-'):
				rv = true;
				break;
			case('.'):
				rv = true;
				break;
			case('_'):
				rv = true;
				break;
			case('~'):
				rv = true;
		}
	}
	return rv;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  is_pct_encoded
 *  Description:  Checks to see if a string is percentage encoded.
 * =====================================================================================
 */
static bool
is_pct_encoded( char *s ) {
	if( ! s ) {
		return false;
	}
	int len = strlen(s);
	bool rv = true;
	if( len != 3){
		return false;
	}
	if( s[0] != '%' || !isxdigit(s[1]) || !isxdigit(s[2])){
		rv = false;
	}
	return rv;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  is_scheme_char
 *  Description:  Check is the character forms a valid URI scheme character.
 * =====================================================================================
 */
static bool 
is_scheme_char( char c )
{
	bool rv = false;
	if( isalpha(c) ) {
		rv = true;
	}
	else if( isdigit(c)){
		rv = true;
	}
	else {
		switch(c){
			case '+':
				rv = true;
				break;
			case '-':
				rv = true;
				break;
			case '.':
				rv = true;
		}
	}
	return rv;
}
