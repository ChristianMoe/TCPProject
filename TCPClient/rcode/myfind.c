/**
 * @file myfind.c
 * Betriebssysteme Aufgabe 1
 *
 * @author Mödlhammer, Chen, Halwax
 * @date 2015/03/06
 * @brief myfind.c
 * @details myfind main module;
 * 
 * myfind.c
 * Aufgabenstellung: Schreiben Sie eine vereinfachte Version von find(1).
 *
 * \param argc the number of arguments
 * \param argv the arguments itselves (including the program name in argv[0])
 *
 * \return EXIT_FAILURE or EXIT_SUCCESS
 *
 * \retval 0 EXIT_SUCCESS on success
 * \retval 1 EXIT_FAILURE on failure
 *
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <libgen.h>

/*
 * -------------------------------------------------------------- includes --
 */
#include "eval.h"
#include "che.h"
#include "hal.h"
#include "moe.h"

/*
 * -------------------------------------------------------------- defines --
 */


/*
 * -------------------------------------------------------------- functions --
 */

/* file and directory scans */
int process_directory_entry(const char *direntry_path, predicate *list_of_predicates, const char *error_prefix);

/* command line args */
/**
 * this function creates a linked list of predicates
 * each predicate represents a parameter passed to myfind 
*/
predicate *create_list_of_predicates(const int argc, const char *argv[]);

/* prints a show usage */
void show_help_with_message(const char *message);

/* internals */
int execute_predicates(predicate *list_of_predicates, const char *direntry_name, const char *direntry_path, const char *error_prefix);

/**
 * This is the main function and entry point
 * 
 * \param argc counter for amount of arguments entered through the command line
 * \param argv char array with programname and command line arguments
 * \return EXIT_SUCCESS on success
 * \return EXIT_FAILURE on error
 * 
 * */
int main(int argc, const char *argv[])
{
    predicate *list_of_predicates;
    
    if (argc == 1) {
        show_help_with_message("No action specified!\n\n");
        return EXIT_SUCCESS;
    }
    
    /* requirements and tests executed by script are different here */
    /*
    if (is_directory(argv[1]) != 1) {
        show_help_with_message("First parameter needs to be a directory!\n\n");
        return EXIT_SUCCESS;
    }
    */
    
    if ((list_of_predicates = create_list_of_predicates(argc, argv)) == NULL) {
        show_help_with_message("parameter missing or invalid usage\n");
        return EXIT_FAILURE;
    }
     
        
    if (process_directory_entry(argv[1], list_of_predicates, argv[0]) == EXIT) {
        return EXIT_FAILURE;
    }    
    
    return EXIT_SUCCESS;
}

/**
 * This function opens a directory and enumerates all entries.
 * For each entry an appropriate function to handle
 * the concrete type is chosen.
 * 
 * \param direntry_path the filesystem path
 * \param list_of_predicates the linked list of predicates which are executed for the directory entry
 * \param error_prefix error message
 * 
 * \return returns an indicator wheater the next predicate should be executed or not
 * \retval CONTINUE
 * \retval STOP
 * \retval EXIT
*/
int process_directory_entry(const char *direntry_path, predicate *list_of_predicates, const char *error_prefix)
{
    DIR *dp = NULL;
    struct dirent *current_direntry = NULL;
    char path[PATH_MAX];
    
    /* stripped filename from path */
    char *direntry_name;    
 
    /* the result of the current eval function */
    /* is either CONTINUE, STOP or EXIT */
    int predicate_evaluation_result = CONTINUE;
    
    /* this code is executed if direntry is of type file, link, pipe, ...
     * but NOT for directories. See "return" for all execution paths */
    if (! is_directory(direntry_path)) {
        if ((direntry_name = basename((char *)direntry_path)) == NULL) {
            fprintf(stderr, "%s: scan_directory failed.\n", error_prefix);
            return EXIT;
        }
        
        predicate_evaluation_result =  execute_predicates(list_of_predicates, direntry_name, direntry_path, error_prefix);
        return predicate_evaluation_result;
    }
    
    /* the following code is only executed if direntry if of type directory */
    if ((dp = opendir(direntry_path)) == NULL) {
        fprintf(stderr, "%s %s: %s\n", error_prefix, direntry_path, strerror(errno));
        return EXIT;
    }

    while ((current_direntry = readdir(dp)) != NULL) {
        /* not interested in current and parent directory */
        if ((strncmp(current_direntry->d_name, ".", 1) == 0) || (strncmp(current_direntry->d_name, "..", 2) == 0)) continue;
        
        /* verify maximum path length */
        if (strlen(direntry_path) + strlen(current_direntry->d_name) + 2 > PATH_MAX) {
            fprintf(stderr, "%s: Exceeded maximum path lenght.\n", error_prefix);
            
            /* since the PATH_MAX may only be exceed for the current entry we can
             * continue with the next entry */
            continue;
        }
        
        /* create a new path with "/" separator */
        snprintf(path, PATH_MAX, "%s/%s", direntry_path, current_direntry->d_name);
        
        /* execute the linked list of predicate functions from 'left' to 'right' */
        predicate_evaluation_result = execute_predicates(list_of_predicates, current_direntry->d_name, path, error_prefix);
        if (predicate_evaluation_result == EXIT) {
            closedir(dp);
            return EXIT;
        }
       
        /* recursive call of process_directory_entry */
        if (is_directory(path) == 1) {
            if (process_directory_entry(path, list_of_predicates, error_prefix) == EXIT) {
                closedir(dp);
                return EXIT;
            }
        }
    }
    
    closedir(dp);
    return CONTINUE;
}
/**
 * This function executes all predicate functions for the current file or directory
 * 
 * \param list_of_predicates linked list which contains all predicates, each corresponds to one valid command line parameter for myfind
 * \param direntry_name name of the directory entry
 * \param direntry_path path of the directory entry
 * \param error_prefix contains the name and the path of the executable (aka myfind)
 * 
 * \var current_predicate pointer to a predicate which contains a function and a pattern. The function is called with the pattern as a parameter
 * \var eval_result result of the evaluation function calls; either CONTINUE or STOP (on errors)
 * 
 * \retval CONTINUE
 * \retval STOP
 * \retval EXIT (non recoverable ERROR)
*/
int execute_predicates(predicate *list_of_predicates, const char *direntry_name, const char *direntry_path, const char *error_prefix) {
    
    /* pointer to the current predicate structure */
    predicate *current_predicate = list_of_predicates;
    
    /* the result of the current predicate function */
    /* is either CONTINUE, STOP or EXIT */
    int predicate_function_result = CONTINUE;

    /* walk through the linked list of predicates and execute the assigned function */
    while ((current_predicate != NULL) && (predicate_function_result == CONTINUE)) {
        predicate_function_result = current_predicate->predicate_function(direntry_name, direntry_path, current_predicate->pattern, error_prefix);
        current_predicate = current_predicate->next;
    }
    
    return predicate_function_result;
}

/**
 * This function creates a linked list out of the evaluation arguments, passed down by the command line;
 * it validates the passed down argument from the command line and adds it with a function call (add_predicate)
 * when it is valid (incl. pattern syntax check)
 * 
 * \param argc counter for amount of arguments passed down via command line 
 * \param argv char array with programname and command line arguments
 * 
 * \var list_of_predicates a pointer to the linked list of predicates
 * \var add_implicit_print adding a -print at the end, if program arguments don't contain a -print already
 * 
 * \return list_of_predicates pointer to linked list of predicates
 * \return NULL (arguments or syntax not valid)
*/
predicate *create_list_of_predicates(const int argc, const char *argv[]) {

    /* root element for linked list of predicate structures */
    predicate *list_of_predicates = NULL;
    int  add_implicit_print = 1;

    /* starting at 2 because 0 -> program name, 1 -> directory or file name */
    int i;
    for (i = 2; i < argc; i++) {
        
        if ((strncmp(argv[i], "-type", 5) == 0))
        {
            /* type requires an param value which MUST NOT start with a "-" */
            i++;
            if (i < argc && ((strncmp(argv[i], "-", 1) != 0))
                && (is_valid_type_option(argv[i])))
                
            {
                add_predicate(&list_of_predicates, &type_predicate, argv[i]);
            }
            else {
                write_formatted("", "Missing or wrong param value for -type!\n");
                return NULL;
            }
        }
        else if ((strncmp(argv[i], "-name", 5) == 0))
        {
            /* name requires an param value which MUST NOT start with a "-" */
            i++;
            if (i < argc && ((strncmp(argv[i], "-", 1) != 0)))
            {
                add_predicate(&list_of_predicates, &name_predicate, argv[i]);
            }
            else {
                write_formatted("", "Missing param value for -name!\n");
                return NULL;
            }
        }
        else if ((strncmp(argv[i], "-path", 5) == 0))
        {
            /* path requires an param value */
            i++;
            if (i < argc && ((strncmp(argv[i], "-", 1) != 0)))
            {
                add_predicate(&list_of_predicates, &path_predicate, argv[i]);
            }
            else {
                write_formatted("", "Missing param value for -path!\n");
                return NULL;
            }
        }
        else if ((strncmp(argv[i], "-user", 5) == 0))
        {
            /* user requires an param value which MUST NOT start with a "-" */
            i++;
            if (i < argc && ((strncmp(argv[i], "-", 1) != 0)))
            {
                add_predicate(&list_of_predicates, &user_predicate, argv[i]);
            }
            else {
                write_formatted("", "Missing param value for -user!\n");
                return NULL;
            }
        }
        else if ((strncmp(argv[i], "-nouser", 7) == 0))
        {
            add_predicate(&list_of_predicates, &nouser_predicate, NULL);
        }
        else if ((strncmp(argv[i], "-print", 6) == 0))
        {
            add_predicate(&list_of_predicates, &print_predicate, NULL);
            /* since we add an implicit print at the end there is no need
             to add a -print if this is the last parameter */
            if (i == (argc - 1)) add_implicit_print = 0;
        }
        else if ((strncmp(argv[i], "-ls", 3) == 0))
        {
            add_predicate(&list_of_predicates, &ls_predicate, NULL);
            add_implicit_print = 0;
        }
        else {
            write_formatted("", "Found unknown parameter!\n");
            return NULL;
        }
    }
 
    if (add_implicit_print == 1) add_predicate(&list_of_predicates, &print_predicate, NULL);
    
    return list_of_predicates;
}

/**
 * This function prints out an error message followed by a list of valid arguments with which the progam 
 * is to be called
 * 
 * \param message a message shown before the actual help text
 * 
 * \return no return value (function is of type void)
 */
void show_help_with_message(const char *message)
{
    if (message != NULL) write_formatted("", "\n%s", message);
    
    write_formatted("","myfind <action>\n");
    write_formatted("","\n");
    write_formatted("","\t-user <name> | <uid>\n");
    write_formatted("","\t-name <pattern>\n");
    write_formatted("","\t-type <bcdpfls>\n");
    write_formatted("","\t-print\n");
    write_formatted("","\t-ls\n");
    write_formatted("","\t-nouser\n");
    write_formatted("","\t-path <pattern>\n");
    write_formatted("","\n");
}

/* testing only */
void traverse_list_of_predicates(predicate *linked_evaluations) {
    predicate *current_eval = linked_evaluations;
    
    while (current_eval != NULL) {
        printf("Param pattern %s\n", current_eval->pattern);
        current_eval = current_eval->next;
    }
}

/* testing only */
void verbose_print_args(int argc, const char *argv[]) {
    int i = 0;
    
    for (i = 0; i < argc; i++) {
        write_formatted("","Arg %d\t %s\n", i, argv[i]);
    }
}



