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

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  pop_segment
 *  Description: get the last segment in the path not including
 *               the initial "/" character (if any) and any subsequent characters up to, 
 *               but an including, the last "/"
 *
 * =====================================================================================
 */
extern char *
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
 *         Name:  shift_segment
 *  Description: get the first path segment from the path, including the initial 
 *               "/" character (if any) and any subsequent characters up to, 
 *               but not including, the next "/", reset
 *
 * =====================================================================================
 */
extern char *
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
extern char *
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
extern char *
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
extern void
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

/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_strcpy
 *  Description:  Copy s2 to s1.
 * =====================================================================================
 */
extern char *
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
extern char *
uri_strcat( char *s1, const char *format, const char *s2)
{
	char *buf;
	asprintf(&buf,format,s1,s2);
	s1 = NULL;
	free(s1);
	s1 = buf;
	return s1;
}

