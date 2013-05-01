/*
 *  utils.h
 *  lab2
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

#ifndef lab2_utils_h
#define lab2_utils_h

#include <stdarg.h>     /* Needed for argument list handling. */
#include <stdlib.h>     /* Needed for process handling. */
#include <string.h>     /* Needed for string manipulations functions. */
#include <stdio.h>      /* Needed for print functions. */

#define NEWLINE_CHAR            '\n'    /* Newline character. */

#define EXIT_VALUE_SUCCESS      0       /* The value which the program will exit with on success. */
#define EXIT_VALUE_ERROR        1       /* The value which the program will exit with on error. */

#define ENV_HOME                "HOME"  /* The variable name of the home path. */

/*
 * Macro to print better error messages and exit the process on error.
 * when parameter r < 0 the process gets killed and a error message is presented.
 */
#define CHECK(r) if(r < 0) {perror(""); fprintf(stderr, "line: %d.\n", __LINE__); exit(EXIT_VALUE_ERROR);}

/*
 * printLine works just like printf, but it prints a NEWLINE_CHAR before and after the text.
 */
void printLine(const char *string, ...);

/*
 * TODO: do me.
 */
int readLine(char *buffer, unsigned int size, FILE *stream);

/*
 * A function that executes a program in a separate process by calling execvp.
 * Arguments to the program to execute should be provided by a NULL-terminated
 * array of NULL-terminated strings. The first string in the array is used to
 * identify which program to execute.
 */
void executeChild(char *args[]);

/* TODO: Rewrite this to be more general.
 * This function will split the specified string _command_ into the array _args_
 * containing all words separated by whitespaces. The whitespaces will not be retained
 * in the array read. If no whitespaces exists, the first item of _args_ array will equal
 * the command string. The args array will be NULL-terminated, and therefore the size of the args
 * array must be the max number of arguments to be read + 1. This is specified with the _size_ parameter.
 * If any error is encountered, the function will exit the program will value EXIT_VALUE_ERROR.
 * If size is less than 1 will treat this as an error.
 */
void explode(char *args[], const unsigned int size, char *command);

#endif
