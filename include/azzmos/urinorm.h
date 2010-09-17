/*
 * =====================================================================================
 *
 *       Filename:  urinorm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15/09/2010 22:18:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */


/* #####   HEADER FILE INCLUDES   ################################################### */
#define __AZZMOS__URINORM_H__
#ifndef __AZZMOS_COMMON_H__
#include <azzmos/common.h>
#endif
#ifndef _AZZMOS_UTILS_H_
#include <azzmos/utils.h>
#endif
#ifndef __AZZMOS_URIOBJ_H__
#include <azzmos/uriobj.h>
#endif

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */
extern char      *uri_remove_dot_segments( char **);
extern int        uri_init_regex( regexpr_t *re);
extern int        uri_parse( uriobj_t *uri, regexpr_t *re, const char *fqp);
extern char      *uri_merge_paths( uriobj_t *base, uriobj_t *rel);
extern uriobj_t  *uri_trans_ref( uriobj_t *base, uriobj_t *ref, bool strict);
extern char      *uri_comp_recomp( uriobj_t *uri);
extern int        uri_norm_scheme( uriobj_t *uri);
extern int        uri_norm_auth( uriobj_t *uri);
extern int        uri_norm_host( uriobj_t *uri);
extern int        uri_norm_port( uriobj_t *uri);
extern int        norm_pct( char **pct);
extern int        uri_norm_port( uriobj_t *uri);
extern int        uri_norm_ipv4( uriobj_t *uri);
extern int        uri_norm_ipv6( uriobj_t *uri);
extern int        uri_norm_auth( uriobj_t *uri);
extern int        uri_auth_sync( uriobj_t *uri);
extern int        uri_normalize( uriobj_t *uri);
extern int        uri_norm_ip( uriobj_t *uri);

