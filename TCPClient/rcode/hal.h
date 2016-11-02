
#ifndef __Betriebssysteme__hal__
#define __Betriebssysteme__hal__
/*
 * -------------------------------------------------------------- includes --
 */
#include <stdio.h>
#include "eval.h"
#include <sys/stat.h>

/*
 * --------------------------------------------------------------- defines --
 */

/*
 * -------------------------------------------------------------- typedefs --
 */

/*
 * ------------------------------------------------------------- function prototypes --
 */
int is_valid_type_option(const char *type_option);
int type_predicate(const char direntry_name[], const char path[], const char pattern[], const char *error_prefix);
int print_predicate(const char direntry_name[], const char path[], const char pattern[], const char *error_prefix);
int ls_predicate(const char direntry_name[], const char path[], const char pattern[], const char *error_prefix);
void build_permission_string(mode_t mode, char buffer[10]);

#endif /* defined(__Betriebssysteme__hal__) */
