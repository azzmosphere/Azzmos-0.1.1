/*
 * =====================================================================================
 *
 *       Filename:  regexpr.h
 *
 *    Description:  PCRE compatibility for azzmos,  regular expressions are performed 
 *                  in the following way,  regex_azz_t object is initilized, then it
 *                  is ran many times, when needed.
 *
 *        Version:  1.0
 *        Created:  30/08/2010 21:25:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#define _AZZMOS_REGEXPR_H_

#ifndef __AZZMOS_COMMON_H__
#include <azzmos/common.h>
#endif
#ifndef __AZZMOS_LIST_H__
#include <azzmos/list.h>
#endif
#ifndef __AZZMOS_UTILS_H__
#include <azzmos/utils.h>
#endif

/* #####   EXPORTED MACROS   ######################################################## */
#ifndef OVECTOR
#define OVECTOR 60
#endif

/* #####   EXPORTED TYPE DEFINITIONS   ############################################## */
struct regexpr_s {
	pcre            *re_code;             /* compiled regular expression */
	pcre_extra     **re_extra;            /* pcre extra string */
	char           **re_subject;          /* the subject string to use */
	int              re_length;           /* length of subject */
	int              re_options;          /* options flags for execution */ 
	int              re_startoffset;      /* where the regular expresion should start from */
	int              re_ovector[OVECTOR]; /* array containing results */
	int              re_ovecsize;         /* size of the ovector will allways be set to OVECTOR */
	long             re_id;               /* identifier of this regex */
	struct list_head re_list;             /* linked list structure */
} typedef regexpr_t;

/* #####   EXPORTED DATA TYPES   #################################################### */

/* #####   EXPORTED VARIABLES   ##################################################### */

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */
extern char *re_strerror( const int err);
extern int   re_init( regexpr_t *re, const int id);
extern int   re_append( regexpr_t *re, const int id);
extern int   re_comp( regexpr_t *re, const int id, const char *pattern, const int options, const unsigned char * tableptr);
extern int   re_exec( regexpr_t *re, const int id);
extern int   re_find( regexpr_t *re, const int id);
