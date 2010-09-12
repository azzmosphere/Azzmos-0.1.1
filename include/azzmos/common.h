/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  This file should be included by all base level MACROS, it will
 *    import all common headers that are used within the project.
 *
 *        Version:  1.0
 *        Created:  26/05/2010 23:08:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */

#define __AZZMOS_COMMON_H__

/* #####   HEADER FILE INCLUDES   ################################################### */
#include <config.h>
#ifndef _STDLIB_H_
#include <stdlib.h>
#endif

#ifndef _CURL_CURL_H
#include <curl/curl.h>
#endif

#ifndef _SYS_SYSLOG_H_
#include <syslog.h>
#endif

#ifndef _STDARG_H_
#include <stdarg.h>
#ifndef _STDARG_H_
#define _STDARG_H_
#endif
#endif

#ifndef _STDIO_H_
#include <stdio.h>
#endif

#ifndef _ERRNO_H_
#include <errno.h>
#endif

#ifndef _STRING_H_
#include <string.h>
#endif

#ifndef _STDBOOL_H_
#include <stdbool.h>
#endif

#ifndef _SYS_TYPES_H_
#include <sys/types.h>
#endif

#ifndef _GETOPT_H_
#include <getopt.h>
#endif

#ifndef LIBPQ_FE_H
#include <libpq-fe.h>
#endif

#ifndef _AZZMOS__LIST_H_
#include <azzmos/list.h>
#endif

/* Use POSIX RE */
#ifndef _PCRE_H
#include <pcre.h>
#endif

#ifndef _PTHREAD_H
#include <pthread.h>
#endif

#ifndef _USER_SIGNAL_H
#include <signal.h>
#endif

#ifndef _TIME_H_
#include <time.h>
#endif

#ifndef _CTYPE_H_
#include <ctype.h>
#endif

/* #####   EXPORTED MACROS   ######################################################## */

/* #####   EXPORTED TYPE DEFINITIONS   ############################################## */

/* #####   EXPORTED DATA TYPES   #################################################### */

/* #####   EXPORTED VARIABLES   ##################################################### */

/* #####   EXPORTED FUNCTION DECLARATIONS   ######################################### */




