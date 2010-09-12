/*
 * =====================================================================================
 *
 *       Filename:  Utils.h
 *
 *    Description:  Functions that are commonly used and fit into a miscellaneois
 *                  category are placed in here.
 *
 *        Version:  1.0
 *        Created:  05/06/2010 22:58:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#define _AZZMOS_UTILS_H_
#ifndef __AZZMOS_COMMON_H__
#include <azzmos/common.h>
#endif

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */
char * usplice( const char *in, unsigned int start, unsigned int end);
char * _macitoa_( int num );
extern void _syslog_print_error( unsigned int tid, char *fname, int lineno, char *m1, char *m2, int pri );
//extern void _syslog_print_error_url( unsigned int tid, char *fname, int lineno, char *m1, char *url, char *m2, int pri );
inline void  reset_file ( FILE *fh );

#define P_SELF (unsigned int) pthread_self()
#define __L_COMMON P_SELF, __FILE__, __LINE__
#define DEBUG(m) syslog( LOG_DEBUG, "%u:%s:%d - %s",  __L_COMMON, m)
#define DEBUG_INT(k,v) syslog( LOG_DEBUG, "%u:%s:%d - %s = '%d'", __L_COMMON, k, v)
#define DEBUG_STR(k,v) syslog( LOG_DEBUG, "%u:%s:%d - %s = '%s'", __L_COMMON, k, v);
#define DEBUG_LNG(k,v) syslog( LOG_DEBUG, "%u:%s:%d - %s = '%li'", __L_COMMON,k, v)
#define ERROR_B(m1,m2) _syslog_print_error(__L_COMMON, m1, (char *) m2, LOG_ERR) 
#define ERROR(m) ERROR_B(m,strerror(errno))
#define ERROR_E(m,e) ERROR_B(m, strerror(e))
#define ERROR_F(f)  ERROR_B("could not print file - %s", f)
//#define ERROR_URL(m1,u,m2)  _syslog_print_error_url(__L_COMMON, m1,u,m2, LOG_ERR)
//#define ERROR_C(m,c) ERROR_B(m, (char *) curl_easy_strerror(c))
//#define ERROR_C_URL(m, u, c) ERROR_URL(m, (char *) u, (char *) curl_easy_strerror(c))
#define WARN_F(f,e)   syslog( LOG_WARNING, "%u:%s:%d - %s reading - %s - %s",__L_COMMON, f, e)
#define WARN(m)       syslog( LOG_WARNING, "%u:%s:%d - %s", __L_COMMON, m)
#define WARN_E(m,e)   syslog( LOG_WARNING, "%u:%s:%d - %s - %s", __L_COMMON, m, strerror(e))
#define INFO(m) syslog( LOG_INFO,  "%u:%s:%d - %s", __L_COMMON, m)
#define CRIT_B(m1,m2) _syslog_print_error(__L_COMMON, m1,m2, LOG_CRIT) 
#define CRIT(m) CRIT_B(m, strerror(errno))
#define CRIT_E(m, e) CRIT_B(m, strerror(e))
//#define CRIT_C(m,c) CRIT_B(m, (char *) curl_easy_strerror(c))


