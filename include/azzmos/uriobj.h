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

/* #####   EXPORTED DATA TYPES   #################################################### */
struct uriobj_s {
	long   uri_id;        /* unique identifier for URI */
	char **uri_scheme;    /* The scheme section */
	char **uri_auth;      /* authority section */
	char **uri_path;      /* path section  */
	char **uri_query;     /* query section */
	char **uri_frag;      /* fragment section */
	time_t uri_mdate;     /* time that URI was last modified */
} typedef uriobj_t;

/* #####   EXPORTED VARIABLES   ##################################################### */

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */
extern char     * uri_remove_dot_segments( char **);
extern int        uri_init_regex( regexpr_t *re);
extern int        uri_parse( uriobj_t *uri, regexpr_t *re, const char *fqp);
extern char     * uri_merge_paths( uriobj_t *base, uriobj_t *rel);
extern uriobj_t * uri_trans_ref( uriobj_t *base, uriobj_t *ref, bool strict);
extern char *     uri_comp_recomp( uriobj_t *uri);
