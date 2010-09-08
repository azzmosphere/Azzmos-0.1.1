/*
 * =====================================================================================
 *
 *       Filename:  Utils.c
 *
 *    Description:  Common tools that are used throughout all programs.
 *
 *        Version:  1.0
 *        Created:  08/08/2010 14:24:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */


#include <azzmos/utils.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  USplice
 *  Description:  splice a character strting from offset start to offset end.  If end is 
 *                NULL then go to end of string. '\0'
 * =====================================================================================
 */
char *
usplice( const char *in, unsigned int start, unsigned int end)
{
	char *out;
	int   l, i = 0;

	if( (char *)end == NULL ){
		end = strlen(in);
	}
	if( (l = end - start) < 0 ){
		return NULL;
	}
	if( end > strlen(in)){
		return NULL;
	}
	if( (out = (char *) malloc((l + 1))) == NULL ){
		return NULL;
	}	
	for( l = start; l < (end + 1); l ++ ){
		out[i++] = in[l];
	}
	if( out[i] != '\0'){
		out[i] = '\0';
	}
	return out;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  _MacItoA_
 *  Description:  Mac does not provide a itoa function, so this function is used, Only if 
 *                the  program is getting compiled on OSX
 * =====================================================================================
 */
char *
_macitoa_( int num )
{
	char *text;
	int result = asprintf( &text, "%d", num );
	if( result < 0 )
	{
		syslog( LOG_ERR, "error converting number to string");
		return NULL;
	}
	return text;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  _syslog_print_error
 *  Description:  Print a error
 * =====================================================================================
 */
extern void 
_syslog_print_error( unsigned int tid, char *fname, int lineno, char *m1, char *m2, int pri )
{
	syslog( pri, "%u:%s:%d - ERROR - %s - %s", tid, fname, lineno, m1, m2);
	perror( m2 );
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  reset_file
 *  Description:  Reset file back to the beginning, file is reopened in "w+" mode.
 * =====================================================================================
 */
inline void
reset_file ( FILE *fh )
{
	rewind(fh);
	fprintf(fh,"%d",EOF);
	freopen(NULL,"w+",fh);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  set_flag
 *  Description:  set the tflag value in the flag variable.
 * =====================================================================================
 */
extern void
set_flag( long *flags, long tflags )
{
	*(flags) |= tflags;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  unset_flag
 *  Description:  Clear the bit flag tflag in the flags variable
 * =====================================================================================
 */
extern void 
unset_flag( long *flags, long tflag)
{
	if( *(flags) & tflag ) {
		*(flags) ^= tflag;
	}
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  is_flag
 *  Description:  Check if the bit flag tflag is set in the flags variable.
 * =====================================================================================
 */
extern bool 
is_flag( const long flags, long tflag )
{
	bool rv = false;
	if( flags & tflag ) {
		rv = true;
	}
	return rv;
}

