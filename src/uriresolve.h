/*
 * =====================================================================================
 *
 *       Filename:  uriresolve.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  16/09/2010 22:18:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */


/* #####   HEADER FILE INCLUDES   ################################################### */
#define __AZZMOS__URIRESOLVE_H__
#ifndef __AZZMOS_COMMON_H__
#include <azzmos/common.h>
#endif
#ifndef _AZZMOS_UTILS_H_
#include <azzmos/utils.h>
#endif

/*****************************************************************************************
 * urinorm will import the uriobj functions so do not re-import them.
 *****************************************************************************************/
#ifndef __AZZMOS__URINORM_H__
#include <azzmos/urinorm.h>
#endif

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */
extern uriobj_t *ref_resolve( uriobj_t *base, char *href, regexpr_t *re, bool strict);
extern int uri_resolve( uriobj_t *uri);