/*
 * =====================================================================================
 *
 *       Filename:  urinorm.c
 *
 *    Description: Web crawler parsing and normalization function of a URI.
 *
 *        Version:  1.0
 *        Created:  15/09/2010 22:15:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#include <urinorm.h>

/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */
#define RE_ID 0
#define RE    "^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?"

/* :TODO:12/09/2010 17:49:55::  */
/************************************************************************************** 
 * All though %7E and ~ are equivalants for the purpose of URI uniquness tilda's are 
 * also transposed to the '~' character.  This is done after percentage encoding 
 * checking is performed.
 **************************************************************************************/
#define RE_TILDA_ID 1

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
static bool is_gen_delims( char c);
static bool is_sub_delim( char c );
static bool is_reserved( char c );
static bool is_unreserved( char c );
static bool is_pct_encoded( char *s );
static bool is_scheme_char( char c );
static bool is_pchar( char c);

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


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_norm_port
 *  Description:  Normalize the port section.  Just check to see if it a number. This 
 *                should only be set if it is different from the scheme, for example http 
 *                on 8080
 * =====================================================================================
 */
extern int
uri_norm_port( uriobj_t *uri)
{
	char *port = URI_CP_PT(uri->uri_port);
	int err = 0,i, len;
	if( port ) {
		len = strlen(port);
		for(i = 0; i < len; i ++){
			if( ! isnumber(port[i])){
				err = EILSEQ;
			}
		}
	}
	return err;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_norm_path
 *  Description:  Normalize the path,  with this section just check for illegal 
 *                characters.  upper and lower case letters are aloud.
 * =====================================================================================
 */
extern int 
uri_norm_path( uriobj_t *uri)
{
	int err = 0,
	    len = 0,
	    i   = 0,
	    n   = 0;
	char *path = URI_CP_PT(uri->uri_path),
	     *ou,
	     *pct;
	if( !path){
		return EINVAL;
	}
	len = strlen(path);
	ou  = (char *) malloc(len + 1);
	pct = (char *) malloc(4);
	if( errno ){
		return errno;
	}
	for(;i < len; i ++){
		if( is_pchar(path[i]) || path[i] == '/'){
			ou[n ++] = path[i];
		}
		else if(path[i] == '%'){
			if( (i + 2) > len){
				err = EILSEQ;
				break;
			}
			pct[0] = path[i ++];
			pct[1] = path[i ++];
			pct[2] = path[i ++];
			pct[3] = '\0';
			err = norm_pct(&pct);
			if( err ){
				break;
			}
			ou[n ++] = pct[0];
			ou[n ++] = pct[1];
			ou[n ++] = pct[2];
		}
		else {
			err = EILSEQ;
			break;
		}
	}
	ou[n] = '\0';
	free(*uri->uri_path);
	*uri->uri_path = strdup(ou);
	return err;	
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_norm_ipv4
 *  Description:  Normalise the uri_ip section if it is IPv4.  This should only be done
 *                if the uri_flags does not have URI_IPV6 and URI_IP is set. 
 *                Normalisation for this is (attempted) to be compliant with section 
 *                3.2.2 of RFC 3986.
 *
 *                if a error is returned by this function then the flag URI_IPINVALID 
 *                should be set.
 *
 *                In this function arguing NULL is considered a EINVAL as it assumes 
 *                the above rules.
 *
 *                IPv4 addresses will be determined by RFC1123 section 2.1, EI
 *                #.#.#.#
 *                  dec-octet   = DIGIT                 ; 0-9
 *                              / %x31-39 DIGIT         ; 10-99
 *                              / "1" 2DIGIT            ; 100-199
 *                              / "2" %x30-34 DIGIT     ; 200-249
 *                              / "25" %x30-35          ; 250-255
 *                
 * =====================================================================================
 */
extern int
uri_norm_ipv4( uriobj_t *uri)
{
	int err = 0, 
	    len, 
	    i,
	    offset = 0,
	    val,
	    segcount = 0;
	char *ip = URI_CP_PT(uri->uri_ip),
	     *segment = (char *) malloc(4);
	if( ! ip ) {
		err = EINVAL;
	}
	else {
		len = strlen(ip);
		for(i = 0;i < len; i ++){
			if( ip[i] == '.'){
				segment[offset] = '\0';
				offset = 0;
				segcount ++;
				if( ! strlen(segment) ){
					err = EILSEQ;
					break;
				}
				if( segcount > 4){
					err = ERANGE;
					break;
				}
				val = atoi(segment);
				switch(strlen(segment)){
					case(1):
						break;
					case(2):
						if(val < 10){
							err = EILSEQ;
						}
						break;
					case(3):
						if(val < 100 || val > 255){
							err = EILSEQ;
						}
						break;

					 /* NOT REACHED */
					default:
						err = EILSEQ;
				}
				if( err ) {
					break;
				}
				segment = strcpy(segment,"");
				continue;
			}
			else {
				if( offset > 3){
					err = ERANGE;
					break;
				}
				if( !isnumber(ip[i])){
					err = EILSEQ;
					break;
				}
				segment[offset] = ip[i];
				offset ++;
			}
		}
	}
	return err;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_norm_auth
 *  Description:  some safe assumptions are aloud here.
 *                1. Because there is only support for http and https no userinfo is 
 *                   required and can be safely ignored.  This is because 404 error
 *                   will produce a request for creditials and thus RFC2616 makes no 
 *                   metion of userinfo. 
 *                2. if the first character of string is not a number or a '[' then we 
 *                   can assume that it is a regname because RFC1035 states that a 
 *                   reg-name must start with a alpha character.
 * =====================================================================================
 */
extern int        
uri_norm_auth( uriobj_t *uri)
{
	int err = 0,
	    n   = 0,
	    i   = 0,
	    len = 0;
	char *auth = URI_CP_PT(uri->uri_auth),
	     *port = NULL,
	     *host ,
	     *buffer,
	     *pct = (char *)malloc(4);
	uri->uri_flags &= ~URI_REGNAME;
	if( !auth ){
		uri->uri_flags |= URI_INVALID;
		return EINVAL;
	}
	if( isalpha(auth[0])){
		uri->uri_flags |= URI_REGNAME;
	}
	else if(auth[0] == '['){
		uri->uri_flags |= URI_IPV6;
		uri->uri_flags |= URI_IP;
		uri->uri_flags &= ~URI_IPINVALID;
		i++;
	}
	else if(isnumber(auth[0])){
		uri->uri_flags |= URI_IP;
	}
	else {
		uri->uri_flags |= URI_INVALID;
		return EILSEQ;
	}
	len = strlen(auth);
	host = (char *) malloc(len + 1);
	if( errno ){
		return errno;
	}
	buffer = host;
	for(;i < len;i++){
		/*
		 * IPV6 end, set it as valid. RFC2732 madates
		 * that it is to be rejected otherwise.  
		 */
		if( auth[i] == ']' && uri->uri_flags & URI_IPV6){
			uri->uri_flags &= ~URI_IPINVALID;
			continue;
		}
		if( auth[i] == ':'){
			port = (char *)malloc((len - i) + 1);
			buffer = port;
			n = 0;
			continue;
		}
		buffer[n] = auth[i];
		n ++;
	}
	if( uri->uri_flags & URI_REGNAME){
		*(uri->uri_host) = NULL;
		free(*uri->uri_host);
		*(uri->uri_host) = strdup(host);
	}
	else if( uri->uri_flags & URI_IP){
		*(uri->uri_ip) = NULL;
		free(*uri->uri_ip);
		*(uri->uri_ip) = strdup(host);
	}
	if( port ){
		*(uri->uri_port) = NULL;
		free(*uri->uri_port);	
		*(uri->uri_port) = strdup(port);
	}
	return err;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_norm_ip
 *  Description:  Determine the address is IPv4 or IPv6 and normalize accordingly.
 * =====================================================================================
 */
extern int 
uri_norm_ip( uriobj_t *uri)
{
	int err = 0;
	if( uri->uri_flags & URI_IPV6){
		err = uri_norm_ipv6(uri);
	}
	else {
		err = uri_norm_ipv4(uri);
	}
	return err;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_norm_ipv6
 *  Description:  Currently this function returns ENOSYS,  as of yet IPv6 support is not
 *                implemented in the downloader.  This should cause the URI_INVALID to 
 *                be set.
 * =====================================================================================
 */
 /* REMAINS TO BE IMPLEMENTED */
extern int
uri_norm_ipv6( uriobj_t *uri)
{
	return ENOSYS;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_auth_sync
 *  Description:  Syncronize the sections that make up the authority. This should be 
 *                done after all normalization.
 * =====================================================================================
 */
extern int        
uri_auth_sync( uriobj_t *uri)
{
	char *auth,
	     *host;
	int len = 0,
	    err = 0;
	if(uri->uri_flags & URI_IP){
		len  = strlen(*uri->uri_ip);
		host = strdup(*uri->uri_ip);
	}
	else if( uri->uri_flags & URI_REGNAME){
		len  = strlen(*uri->uri_host);
		host = strdup(*uri->uri_host);
	}
	else {
		err = EINVAL;
	}
	if( ! err ) {
		if( uri->uri_flags & URI_IPV6){
			if( *uri->uri_port){
				asprintf(&auth,"[%s]:%s",host, *uri->uri_port);
			}
			else {
				asprintf(&auth,"[%s]",host);
			}
		}
		else {
			if(*uri->uri_port){
				asprintf(&auth, "%s:%s",host, *uri->uri_port);
			}
			else {
				asprintf(&auth, "%s",host);
			}
		}
		if( ! err ) {
			*(uri->uri_auth) = NULL;
			free(*uri->uri_auth);
			*(uri->uri_auth) = strdup(auth);
		}
	}
	return err;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_normalize
 *  Description:  Normalize all section of the URI.
 * =====================================================================================
 */
extern int        
uri_normalize( uriobj_t *uri)
{
	int err = uri_norm_scheme(uri);
	if( ! err ) {
		err = uri_norm_auth(uri);
	}
	if( ! err ) {
		if( uri->uri_flags & URI_REGNAME) {
			err = uri_norm_host(uri);
		}
	}
	if( ! err && *(uri->uri_port)){
		err = uri_norm_port(uri);
	}
	if( ! err && uri->uri_flags & URI_IP){
		err = uri_norm_ip(uri);
	}
	if( ! err ){
		err = uri_auth_sync(uri);
	}
	if( ! err ){
		err = uri_norm_path(uri);
	}
	return err;
}



/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */
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


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  is_pchar
 *  Description:  Test if a character is a pchar as desribed by RFC3986.  A pchar
 *                is: unreserved / pct-encoded / sub-delims / ":" / "@", if character
 *                is a pchar return true otherwise return false.
 * =====================================================================================
 */
static bool
is_pchar( char c)
{
	bool result  = false;
	if( is_unreserved(c) || is_sub_delim(c) ){
		result = true;
	}
	else if( c == ':' || c == '@'){
		result = true;
	}
	return result;
}
