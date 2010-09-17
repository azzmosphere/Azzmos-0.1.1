/*
 * =====================================================================================
 *
 *       Filename:  uriresolve.c
 *
 *    Description: URI resolition functions, these functions resolve a URI to its
 *                 full path when given a base.  They are used as part of the 
 *                 downloader
 *
 *        Version:  1.0
 *        Created:  16/09/2010 22:15:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aaron Spiteri
 *        Company:  
 *
 * =====================================================================================
 */

/* #####   HEADER FILE INCLUDES   ################################################### */
#include <uriresolve.h>

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ############################ */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ref_resolve
 *  Description:  This function is used by the downloader to normalize and resolve a
 *                href in a given HTML page to a URI object.  The base should be the
 *                URI object that contains the HREF and strict refers to RFC3986 section
 *                5.2.2 which describes the behavour the algoritm should take if the 
 *                scheme section of the URI is absent.
 *
 *                The calling routine of this function should check the return objects
 *                uri_flags for URI_INVALID, this should not be a critical error rather
 *                it should instruct qu_ok_download not to download the URI.
 *                
 *                It should also check to see if the return value is NULL, if it is then
 *                a error should be raised printing errno string reference as this will
 *                be a memory allocation problem.
 * =====================================================================================
 */
extern uriobj_t *
ref_resolve( uriobj_t *base, char *href, regexpr_t *re, bool strict)
{
	int err = 0;
	uriobj_t trans,
			*ref = (uriobj_t *) malloc(sizeof(uriobj_t));
	if( !ref) {
		return NULL;
	}
	err = uri_parse(ref, re, href);
	if( err ) {
		ERROR_E("parsing URI href", err);
		ref->uri_flags |= URI_INVALID;
	}
	return ref;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  uri_resolve
 *  Description:  Use the DNS server or underlying OS resolver to fill out URI attributes.
 *                On error the gai_error code is returned, otherwise the return value is 
 *                '0',  if the error is a standard error then EAI_SYSTEM is returned and
 *                errno is set.
 * =====================================================================================
 */
extern int 
uri_resolve( uriobj_t *uri)
{
	int gai_error = 0;
	struct addrinfo hints,
				   *addr;
	char *host, 
		 *serv,
		 *ip;
	bzero(&hints, sizeof(hints));
	
	/* For this application allways take TCP sockets */
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	serv = *(uri->uri_scheme);
	hints.ai_family = PF_UNSPEC;
	
	/* calculate what needs to be returned */
	if( uri->uri_flags & URI_REGNAME){
		host = *(uri->uri_host);
	}
	else if(uri->uri_flags & URI_IP){
		host = *(uri->uri_ip);
		hints.ai_flags = AI_NUMERICHOST|AI_CANONNAME;
		if( uri->uri_flags & URI_IPV6){
			hints.ai_family = AF_INET6;
		}
		else {
			hints.ai_family = AF_INET;
		}
	}
	else {
		errno = EINVAL;
		gai_error = EAI_SYSTEM;
	}
	
	/* Make call to systems resolver */
	if( ! gai_error ) {
		
			hints.ai_flags |= AI_CANONNAME;
	
			gai_error = getaddrinfo(host, serv, &hints, &addr);
	}
	
	/* If call successfull populate the URI object */
	if( ! gai_error) {
		if( ! *uri->uri_host ){
			*uri->uri_host = addr->ai_canonname;
		}
		else {
			ip = inet_ntop(AF_INET, &(((struct sockaddr_in *)addr->ai_addr)->sin_addr),
                    ip, BUFSIZ);
			*uri->uri_ip = strdup(ip);
		}
		*uri->uri_addr = addr;
	}
	return gai_error;
}