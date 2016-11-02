/**
 * @file hal.c
 * Betriebssysteme - add_test and is_directory function
 *
 * @author Thomas Halwax
 * @details This module contains three functions. add_predicate is to add a new predicate to the linked list of predicates. The function is_directory is to detect if the current entry is a directory or a file. write_formatted()
 * @date 2015/06/03
 *
 * @version 1.0
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "eval.h"
#include <errno.h>
#include <string.h>
#include <fcntl.h>

/**
 *
 * \brief add_predicate adds a new predicate struct to the linked list of existing predicates.
 * If list_of_predicates is NULL, a new list is created.
 * 
 * \param list_of_predicates  a linked list of predicates
 * \param predicate_function  a pointer to a function of type func which will be called for
 *                            each directory entry regardless of file, directory, sym link, ...
 *                            a function can handle one type of parameter (-name, -path, ...)
 * \param pattern             a string pattern which represents the option for the parameter
 *
 * \return no return value for this function
 */
void add_predicate(predicate **list_of_predicates, func predicate_function, const char pattern[]) {
    predicate *current_predicate = NULL;
    predicate *new_predicate = NULL;

    if ((new_predicate = (predicate*)malloc(sizeof(predicate))) == NULL) {
        fprintf(stderr, "cannot allocate memory for new predicate: %s\n", strerror(errno));
        return;
    }

    new_predicate->predicate_function = predicate_function;
    new_predicate->pattern = pattern;
    new_predicate->next = NULL;
 
    /* this is the first predicate */
    if (*list_of_predicates == NULL) {
        *list_of_predicates = new_predicate;
        current_predicate = new_predicate;
    }
    else {
        /* there is already a list of predicates, navigate to last entry */
        current_predicate = *list_of_predicates;
        if (current_predicate != NULL) {
            while (current_predicate->next != NULL) {
                current_predicate = current_predicate->next;
            }
        }
        
        /* append current predicate to the (linked) list */
        current_predicate->next = new_predicate;
        current_predicate = new_predicate;
    }
}


/**
 * \brief This function checks if it is a directory
 *
 * \param path  char of direcotry name
 *
 * \return returns value depending whether directory or not
 * \retval 1 if it is a directory
 * \retval 0 if it is not a directory
 * \retval -1 on error
 */
int is_directory(const char *path){
    struct stat direntry;
    if (stat(path, &direntry) == -1) {
        fprintf(stderr, "%s: %s\n", path, strerror(errno));
        return EXIT;
    }    
    return S_ISDIR(direntry.st_mode);
}

/**
 * \brief This function prints a formatted string using vprintf
 *
 * \param formatted_string
 * 
 * \return returns EXIT or CONTINUE
 * \retval return_code EXIT on failure
 * \retval return_code CONTÍNUE on success
 */
int write_formatted(const char *error_prefix, const char *formatted_string, ...) {
    
    int return_code = CONTINUE;
    
    va_list args;
    
    va_start(args, formatted_string);
    if (vprintf(formatted_string, args) < 0) {
        fprintf(stderr, "%s write_formatted(): %s\n", error_prefix, strerror(errno));
        return_code = EXIT;
    }
    va_end(args);
    
    return return_code;
}


