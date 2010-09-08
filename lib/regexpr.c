/*
 * =====================================================================================
 *
 *       Filename:  regexpr.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2010 21:16:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#include <azzmos/regexpr.h>

/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ################################### */

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ############################### */
static void re_set_default( regexpr_t *re, int id );

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  re_strerror
 *  Description:  convert pcre error offset into a character array terminated with '\0'
 * =====================================================================================
 */
extern char *
re_strerror( const int err)
{
	char *errstr = NULL;
	return errstr;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  re_init
 *  Description:  create the initial structure, initilizing the list.  This includes 
 *                setting any default values.
 * =====================================================================================
 */
extern int   
re_init( regexpr_t *re, const int id)
{
	errno = 0;
	re_set_default(re, id);
	if( errno ) {
		return errno;
	}
	INIT_LIST_HEAD(&re->re_list);
	return errno;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  re_append
 *  Description:  create a new re object and append it to the list. Return 0 on success
 *                or a value above 0 on failure.
 * =====================================================================================
 */
extern int   
re_append( regexpr_t *re, const int id)
{
	regexpr_t *new_re = (regexpr_t *) malloc( sizeof(regexpr_t));
	if( ! new_re ) {
		ERROR("could not initlize new structure");
		return errno;
	}
	re_set_default( new_re, id );
	list_add( &(new_re->re_list), &(re->re_list));
	re = new_re;
	return 0;
}
/* #####   FUNCTION DEFINITIONS  -  LOCAL TO THIS SOURCE FILE   ##################### */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  re_set_defaults
 *  Description:  sets the default values for the regexpr_t structure.
 * =====================================================================================
 */
static void 
re_set_default( regexpr_t *re, int id )
{
	if( ! re->re_extra) {
		re->re_extra = (pcre_extra **) malloc( sizeof(pcre_extra *));
	}
	if( ! re->re_subject){
		re->re_subject = (char **) malloc( sizeof(char *));
	}
	re->re_ovecsize = OVECTOR;
	re->re_id = id;
	*(re->re_extra) = (pcre_extra *) NULL;
	*(re->re_subject) = NULL;
	re->re_options  = 0;
	re->re_startoffset = 0;	
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  re_find
 *  Description:  search through the RE list, find the re that has its re_id equal to
 *                id and return.  If one can not be found then return ENOATTR 
 * =====================================================================================
 */
extern int   
re_find( regexpr_t *re, const int id)
{
	struct list_head *pos;
	regexpr_t *tmp;
	bool found = false;
	int  rv = 0;

	if( re->re_id == id ) {
		return rv;
	}
	list_for_each(pos, &re->re_list){
		tmp = list_entry(pos, regexpr_t, re_list);
		if( tmp->re_id == id ) {
			re = tmp;
			found = true;
			break;
		}
	}
	if( ! found ) {
		rv = ENOATTR;
	}
	return rv;
}

extern int   
re_comp( regexpr_t *re, const int id, const char * pattern, const int options, const unsigned char * tableptr)
{
	int   erroroffset = re_find( re, id);
	char *errptr;
	if( erroroffset ) {
		return erroroffset;
	}
	re->re_code = pcre_compile(pattern, options, (const char **)&errptr, &erroroffset, tableptr);
	if( erroroffset ) {
		ERROR_B( errptr, pattern);
	}
	return erroroffset;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  re_exec
 *  Description:  Execute the regular expression.
 * =====================================================================================
 */
extern int   
re_exec( regexpr_t *re, const int id)
{
	int erroroffset = re_find( re, id);
	if( erroroffset ){
		return erroroffset;
	}
	erroroffset = pcre_exec(re->re_code, *re->re_extra, *re->re_subject, re->re_length, re->re_startoffset, re->re_options, re->re_ovector, re->re_ovecsize);
	return erroroffset;
}
