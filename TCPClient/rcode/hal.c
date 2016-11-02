/**
 * @file hal.c
 * Betriebssysteme - Betriebssysteme - type, print and ls function
 *
 * @author Thomas Halwax, ic14b050
 * @details This module performs the -type -print and -ls functions and evaluations for the myfind program.
 * @date 2015/06/03
 *
 * @version 1.0
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */

#include "hal.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>

/**
 * This macro is used to check if write_formatted returns an error. If so
 * the function returns EXIT
*/
# define EXIT_ON_ERROR(return_value) if (return_value == EXIT) return EXIT


/**
 * This function checks if the entered types are valid inputs by the user
 * 
 * \param type_option the passed down type option via command line
 * 
 * \return 1 returns 1 if valid (to proceed with the program)
 * \return 0 returns 0 if invalid (to stop the program)
 */

int is_valid_type_option(const char *type_option) {
        
    return (strncmp(type_option, "b", 1) == 0)
    || (strncmp(type_option, "c", 1) == 0)
    || (strncmp(type_option, "d", 1) == 0)
    || (strncmp(type_option, "f", 1) == 0)
    || (strncmp(type_option, "l", 1) == 0)
    || (strncmp(type_option, "p", 1) == 0)
    || (strncmp(type_option, "s", 1) == 0);
}

/**
 *
 * type_predicated checks, if the given type pattern matches the directory entry's type
 *
 * \param direntry_name[] (IN) the name of the file or directory to evaluate (IN)
 * \param path[] the full path (relative or absolute) of the direntry_name (IN)
 * \param pattern the pattern to avaluate
 * \return CONTINUE (1) if the evaluation of the pattern with direntry_name or path is
 *         successful, STOP (0) otherwise
 *
 * \retval CONTINUE on success
 * \retval STOP otherwise
 */
int type_predicate(
              const char direntry_name[] __attribute__ ((unused))
              , const char path[]
              , const char pattern[] __attribute__ ((unused))
              , const char *error_prefix
              )
{
    
    struct stat buffer;
    char *mode;
    
    if (lstat(path, &buffer) == -1) {
        fprintf(stderr, "%s %s: %s\n", error_prefix, path, strerror(errno));
        return STOP;
    }
    
    if (S_ISBLK(buffer.st_mode)) mode = "b";
    else if (S_ISCHR(buffer.st_mode)) mode = "c";
    else if (S_ISDIR(buffer.st_mode)) mode = "d";
    else if (S_ISREG(buffer.st_mode)) mode = "f";
    else if (S_ISLNK(buffer.st_mode)) mode = "l";
    else if (S_ISFIFO(buffer.st_mode)) mode = "p";
    else if (S_ISSOCK(buffer.st_mode)) mode = "s";
    else {
        fprintf(stderr, "%s unknown type!\n", error_prefix);
        return STOP;
    }
    
    if ((strncmp(mode, pattern, 1) == 0)) {
        return CONTINUE;
    }
    
    return STOP;
}

/**
 *
 * print_predicate writes the path of the directory entry to stdout. It returns
 * the return value of write_formatted which only fails if there was an error
 * printing to stdout.
 * 
 * \param direntry_name[] UNUSED
 * \param path[] path to this entry
 * \param pattern[] UNUSED
 * \param error_prefix 
 *
 * \return success indicateor usually CONTINUE
 * \retval CONTINUE
 * \retval EXIT if printing to stdout fails
  */
int print_predicate(
          const char direntry_name[] __attribute__ ((unused))
          , const char path[]
          , const char pattern[] __attribute__ ((unused))
          , const char *error_prefix
          )
{
    return write_formatted(error_prefix, "%s\n", path);
}

/**
 * This function implements the -ls parameter, displaying detailed information about the file
 *  system entry
 * 
 * \param direntry_name[] UNUSED
 * \param path[] path to this entry
 * \param pattern[] UNUSED
 * \param error_prefix stderr string
 * 
 * \return returns information to continue or stop
 * \retval CONTINUE
 * \retval EXIT if printing to stdout fails
 */
int ls_predicate(
       const char direntry_name[] __attribute__ ((unused))
       , const char path[]
       , const char pattern[] __attribute__ ((unused))
       , const char *error_prefix
       )
{   
    struct stat     dir_entry_info;
    struct tm       *tm;
    struct passwd   *passwd_entry;
    struct group    *group_entry;
    char            time_string[6];
    char            month_string[4];
    char            permission_string[10];
    char            *sym_link_target_name;
    
    
    if (lstat(path, &dir_entry_info) == -1) return STOP;
    
    EXIT_ON_ERROR(write_formatted(error_prefix, "%6lu", (unsigned long)dir_entry_info.st_ino));
    
    /*
     man page für find: The block counts are of 1K blocks, unless the environment variable POSIXLY_CORRECT is set, in which case 512-byte blocks are used.
    */
    if (getenv("POSIXLY_CORRECT")) {
        EXIT_ON_ERROR(write_formatted(error_prefix, " %4lu", (unsigned long)dir_entry_info.st_blocks));
    }
    else {
        EXIT_ON_ERROR(write_formatted(error_prefix, " %4lu", (unsigned long)(dir_entry_info.st_blocks / 2)));
    }
    
    /* permissions */
    build_permission_string(dir_entry_info.st_mode, permission_string);
    EXIT_ON_ERROR(write_formatted(error_prefix, " %10s", permission_string));
    
    /* number of hardliks */
    EXIT_ON_ERROR(write_formatted(error_prefix, " %3d", dir_entry_info.st_nlink));
    
    /* show username or UID */
    if ((passwd_entry = getpwuid(dir_entry_info.st_uid)) != NULL) {
        EXIT_ON_ERROR(write_formatted(error_prefix, " %-8s", passwd_entry->pw_name));
    }
    else {
        EXIT_ON_ERROR(write_formatted(error_prefix, " %-8d", dir_entry_info.st_uid));
    }
    
    /* show groupname or GID */
    if ((group_entry = getgrgid(dir_entry_info.st_gid)) != NULL) {
        EXIT_ON_ERROR(write_formatted(error_prefix, " %-8s", group_entry->gr_name));
    }
    else {
        EXIT_ON_ERROR(write_formatted(error_prefix, " %-8d", dir_entry_info.st_gid));
    }
    
    EXIT_ON_ERROR(write_formatted(error_prefix, "%9lu", (unsigned long)dir_entry_info.st_size));
    
    /* OSX is a litte different :-) */
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
    tm = localtime(&dir_entry_info.st_mtimespec.tv_sec);
#else
    tm = localtime(&dir_entry_info.st_mtime);
#endif    
    
    /* %b     The abbreviated month name according to the current locale. */
    strftime(time_string, sizeof(time_string), "%H:%M", tm);
    strftime(month_string, sizeof(month_string), "%b", tm);

    EXIT_ON_ERROR(write_formatted(error_prefix, " %3s %d %5s", month_string, tm->tm_mday, time_string));
    
    /* print the target the link is pointing to */
    if (S_ISLNK(dir_entry_info.st_mode) == 1) {
        if ((sym_link_target_name = alloca((dir_entry_info.st_size + 1) * sizeof(char))) != NULL) {
            sym_link_target_name[dir_entry_info.st_size] = '\0';
            if (readlink(path, sym_link_target_name, dir_entry_info.st_size) != -1) {
                EXIT_ON_ERROR(write_formatted(error_prefix, " %s -> %s", path, sym_link_target_name ));
            }
        }
    }
    else {
        EXIT_ON_ERROR(write_formatted(error_prefix, " %-s", path));
    }
    EXIT_ON_ERROR(write_formatted(error_prefix, "\n"));
    
    return CONTINUE;
}

/**
 * This function builds up the permission string, which a file system entry has (rwx; entry type, read-write-execute for user, group and others; s for set to uid...)
 * 
 * \param mode of directory entry
 * 
 * \return no return value
 */
void build_permission_string(mode_t mode, char buffer[10]) {
    
    /* ftype contains the 'file' type */
    char ftype = '?';
    if (S_ISREG(mode)) ftype = '-';
    if (S_ISLNK(mode)) ftype = 'l';
    if (S_ISDIR(mode)) ftype = 'd';
    if (S_ISBLK(mode)) ftype = 'b';
    if (S_ISCHR(mode)) ftype = 'c';
    if (S_ISFIFO(mode)) ftype = '|';
    
    sprintf(buffer, "%c%c%c%c%c%c%c%c%c%c",
             ftype,
             (mode & S_IRUSR) ? 'r' : '-', (mode & S_IWUSR) ? 'w' : '-',
             (mode & S_IXUSR) ?
                ((mode & S_ISUID) ? 's' : 'x') :
                ((mode & S_ISUID) ? 'S' : '-'),
             (mode & S_IRGRP) ? 'r' : '-', (mode & S_IWGRP) ? 'w' : '-',
             (mode & S_IXGRP) ?
                ((mode & S_ISGID) ? 's' : 'x') :
                ((mode & S_ISGID) ? 'S' : '-'),
             (mode & S_IROTH) ? 'r' : '-', (mode & S_IWOTH) ? 'w' : '-',
             (mode & S_IXOTH) ?
                ((mode & S_ISVTX) ? 't' : 'x') :
                ((mode & S_ISVTX) ? 'T' : '-'))
    ;
}





