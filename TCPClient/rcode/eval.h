#include <stdarg.h>


#ifndef Betriebssysteme_eval_h
#define Betriebssysteme_eval_h
/*
 * --------------------------------------------------------------- defines --
 */


#define CONTINUE 1
#define STOP 0
#define EXIT -1

/*
 * -------------------------------------------------------------- typedefs --
 */

/* this is a type definition for a function pointer */
/* the paramters are: */
/* direntry_name, direntry_path, pattern, error_prefix */
typedef int (*func)(const char[], const char[], const char[], const char[]);

/* the predicate is used to handle the function calls for each parameter which */
/* is passed to the program */
typedef struct predicate {
    func                 predicate_function;
    const char           *pattern;
    struct predicate     *next;
} predicate;

/*
 * ------------------------------------------------------------- function prototypes --
 */

/* for debugging purposes only */
void traverse_list_of_predicates(predicate *list_of_predicates);

/* helper for test functions */
void add_predicate(predicate **list_of_predicates, func predicate_function, const char pattern[]);

/* common helper */
int is_directory(const char *path);

/* used to wrap the printf function which may fail */
int write_formatted(const char *error_prefix, const char *formatted_string, ...);

#endif
