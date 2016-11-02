/**
 * @file che.c
 * Betriebssysteme - test user/no-user
 *
 * @author Ching-Chih Chen, ic14b049
 * @details This module performs the tests for -user and -nouser arguments.
 * @date 2015/06/03
 * 
 * @version 1.0
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
#include "che.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include "eval.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <alloca.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
/*
 * ------------------------------------------------------------------------functions --
 */

/**
 * This function checks if the username loaded from the parameters exists in
 * in the systems passwd file
 * 
 * \param file_name[] UNUSED
 * \param directory_name[] char string of absolute path incl file name
 * \param uname pattern[] of the -user function (username or uid)
 * \param error_prefix stderr string
 * 
 * \return CONTINUE on success
 * \return STOP on failure of pattern check
 * 
 * */
int user_predicate(
         const char file_name[] __attribute__ ((unused))
         , const char directory_name[]
         , const char uname[]
         , const char *error_prefix
         )
{
    struct passwd *pPasswdEntry;
    struct stat pInode;

    uid_t uname_as_uid;
    char *post_parsed_uid;

    errno = 0;
    if (stat(directory_name, &pInode) == -1)  {
        fprintf(stderr, "%s: user() %s\n", error_prefix, strerror(errno));
        return STOP;
    }
    
    errno = 0;
    pPasswdEntry = getpwnam(uname);
    if (errno != 0) {
        fprintf(stderr, "%s: user() %s\n", error_prefix, strerror(errno));
        return STOP;
    }

    /* user found by name, compare UID */
    if (pPasswdEntry != NULL) {
        if (pPasswdEntry->pw_uid == pInode.st_uid) {
            return CONTINUE;
        }
        return STOP;
    }
    
    /* pPasswortEntry is NULL, trying uname as UID */
    errno = 0;
    uname_as_uid = (uid_t)strtol(uname, &post_parsed_uid,  10) ;
    if (errno == ERANGE) {
        fprintf(stderr, "%s: user() %s\n", error_prefix, strerror(errno));
        return STOP;
    }
    
    if (uname == post_parsed_uid) {
        fprintf(stderr, "%s: user() %s does not exist.\n", error_prefix, uname);
        /* uname is not found as a username and it has no digits so it's not a UID */
        /* this is a hard program exit */
        /* I'd prefer STOP but reference implementation uses EXIT */
        return EXIT;
    }
    
    if (uname_as_uid == pInode.st_uid) {
        return CONTINUE;
    }
    
    return STOP;
}

/**
 * This function checks if the current file owner is a non existing user account
 * 
 * \param file_name[] UNUSED
 * \param directory_name[] char string of absolute path incl file name
 * \param uname[] UNUSED
 * \param error_prefix sdterr string
 * 
 * \return CONTINUE on success
 * \return STOP on failure on check (username exists in passwd)
 * 
 * */
int nouser_predicate(
           const char file_name[] __attribute__ ((unused))
           , const char directory_name[]
           , const char uname[] __attribute__ ((unused))
           , const char *error_prefix
           )
{
    struct passwd *pPasswdEntry = NULL;
    struct stat pInode;
    
    if (stat(directory_name, &pInode) == -1)  {
        fprintf(stderr, "%s: nouser() %s\n", error_prefix, strerror(errno));
        return STOP;
    }

    pPasswdEntry = getpwuid(pInode.st_uid);
    if(pPasswdEntry == NULL) return CONTINUE;  /*no entry found with specified uid*/
  		
    return STOP;
}
