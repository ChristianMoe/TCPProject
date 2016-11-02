

#ifndef __Betriebssysteme__che__
#define __Betriebssysteme__che__
/*
 * --------------------------------------------------------------- defines --
 */

/*
 * -------------------------------------------------------------- typedefs --
 */

/*
 * --------------------------------------------------- function prototypes --
 */
int user_predicate(const char file_name[], const char directory_name[], const char uname[], const char *error_prefix);
int nouser_predicate(const char file_name[], const char directory_name[], const char uname[], const char *error_prefix);

#endif /* defined(__Betriebssysteme__che__) */



