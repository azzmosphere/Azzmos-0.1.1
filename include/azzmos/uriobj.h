/*
 * =====================================================================================
 *
 *       Filename:  uriobj.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  28/08/2010 13:46:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */


/* #####   HEADER FILE INCLUDES   ################################################### */
#define __AZZMOS_URIOBJ_H__
#ifndef __AZZMOS_COMMON_H__
#include <azzmos/common.h>
#endif
#ifndef _AZZMOS_UTILS_H_
#include <azzmos/utils.h>
#endif
#ifndef _AZZMOS_REGEXPR_H_
#include <azzmos/regexpr.h>
#endif

/* #####   EXPORTED MACROS   ######################################################## */

/* #####   EXPORTED TYPE DEFINITIONS   ############################################## */
#define URI_REGNAME    0x01
#define URI_IPINVALID  0x02
#define URI_IPV6       0x04
#define URI_INVALID    0x08
#define URI_IP         0x10

/* #####   EXPORTED DATA TYPES   #################################################### */
struct uriobj_s {
	long   uri_id;        /* unique identifier for URI */
	char **uri_scheme;    /* The scheme section */
	char **uri_auth;      /* authority section */
	char **uri_path;      /* path section  */
	char **uri_query;     /* query section */
	char **uri_frag;      /* fragment section */
	char **uri_host;      /* hostname of URI */
	char **uri_port;      /* uri port number */
	char **uri_ip;        /* IP address */
	time_t uri_mdate;     /* time that URI was last modified */
	long   uri_flags;     /* various flags for the uri */
} typedef uriobj_t;

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */
extern char *pop_segment( char **path);
extern char *shift_segment( char **path, const int offset);
extern char *get_next_segment( char **path);
extern char *replace_prefix( char **path);
extern void  init_uriobj_str( uriobj_t *uri);
extern char *uri_strcpy( char **s1, const char *s2);
extern char *uri_strcat( char *s1, const char *format, const char *s2);


