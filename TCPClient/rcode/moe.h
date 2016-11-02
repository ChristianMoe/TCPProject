
#ifndef Betriebssysteme_moe_H
#define Betriebssysteme_moe_H
/*
 * --------------------------------------------------------------- defines --
 */

/*
 * -------------------------------------------------------------- typedefs --
 */

/*
 * ------------------------------------------------------------- function prototypes --
 */
int name_predicate(const char file_name[], const char directory_name[], const char pattern[], const char *error_prefix );
int path_predicate(const char file_name[], const char directory_name[], const char pattern[], const char *error_prefix );

#endif /* defined(Betriebssysteme_moe_H) */
