/**
 * @file moe.c
 * Betriebssysteme - test name/path pattern file.
 *
 * @author Christian Mödlhammer,ic14b027
 * @details This module performs the -name and -path evaluations for the myfind program.
 * @date 2015/06/03
 *
 * @version 1.0
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
#include "moe.h"
#include "eval.h"
#include <stdio.h>
#include <fnmatch.h>

/*
 * ------------------------------------------------------------- functions --
 */

/**
 * \brief This function checks if the name of the file matches the pattern
 *
 * \param file_name[] char string of the file name
 * \param path[] UNUSED
 * \param pattern[] char string of pattern
 * \param error_prefix UNUSED
 *
 * \return returns information to continue or stop
 * \retval CONTINUE on successful pattern match
 * \retval STOP on failure of pattern check
 *
 */

int name_predicate(
                const char file_name[]
              , const char path[] __attribute__ ((unused))
              , const char pattern[]
              , const char *error_prefix __attribute__ ((unused))
              )
{
	if (fnmatch(pattern,file_name,FNM_NOESCAPE|FNM_PERIOD)==0) return CONTINUE;
    return STOP;
}
/**
 * \brief This function checks if the name of the path matches the pattern
 *
 * \param file_name[] UNUSED
 * \param path[] char string of absolute path incl file name
 * \param pattern[] UNUSED
 * \param error_prefix stderr string
 *
 * \return returns information to continue or stop
 * \retval CONTINUE on successful pattern match
 * \retval STOP on failure of pattern check
 *
 */

int path_predicate(
                const char file_name[] __attribute__ ((unused))
              , const char path[]
              , const char pattern[]
              , const char *error_prefix __attribute__ ((unused))
              )
{
	if (fnmatch(pattern,path,FNM_NOESCAPE)==0) return CONTINUE;
    return STOP;
}
/*
 * =================================================================== eof ==
 */
