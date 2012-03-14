#ifndef DEFINE_STDIO_HELPER_HEADER
#define DEFINE_STDIO_HELPER_HEADER

/*
 *********************************
 * file_foreach.h - stdio poetry *
 * --------------                *
 * Similar to Ruby's IO#each     *
 *********************************
 */

#include <stdbool.h>
#include <stdio.h>

/* 
 * Invoke a callback on each line of the file. A line has a maximum length of 
 * line_max_len, but could be less if a newline is encountered. Stops execution
 * if callback returns false.
 * @pre : self is opened in write mode, line_max_len>0, callback != NULL
 * @post: return true, or false if an allocation error occured or
 *        if the callback returned false.        
 */
bool fnforeach(FILE *self, int line_max_len, bool (*callback)(char *line));

#endif
