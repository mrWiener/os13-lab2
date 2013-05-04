/*
 *  utils.h
 *  lab2
 *
 *  utils.h holds utility functions to be used by the shell program. The utility functions
 *  consists of reading and printing strings, executing child proccesses, string manipulation
 *  and command string checks. Also a macro for checking function return error values is defined.
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
#include <errno.h>      /* Needed for error handling. */
#include <sys/types.h>	/* Needed for process handling. */
#include <sys/wait.h>	/* Needed for wait process handling. */

#define NEWLINE_CHAR            '\n'    /* Newline character. */

#define EXIT_VALUE_SUCCESS      0       /* The value which the program will exit with on success. */
#define EXIT_VALUE_ERROR        1       /* The value which the program will exit with on error. */

#define ENV_HOME                "HOME"  /* The variable name of the home path. */

#define BACKGROUND_CHAR         '&'     /* The char that specifies that a command should be runned in background. */

#define CHILD_FOREGROUND        1       /* The child should be executed in foreground, blocking that is. */
#define CHILD_BACKGROUND        2       /* The child should be executed in background, non-blocking that is. */

#define BACKGROUND_REMOVE_CHAR  1       /* Tells isBackgroundRequested function to remove BACKGROUND_CHAR from args list. */
#define BACKGROUND_KEEP_CHAR    2       /* Tells isBackgroundRequested function to keep BACKGROUND_CHAR in args list. */

#define STANDARD_INPUT          0       /* The standard input file descriptor. */
#define STANDARD_OUTPUT         1       /* The standard output file descriptor. */
#define STANDARD_ERROR          2       /* The standard error file descritpor. */

#define PIPE_IN                 0       /* The read side of the pipe file descriptor. */
#define PIPE_OUT                1       /* The print side of the pipe file descriptor. */

/*
 * Macro to print better error messages and exit the process on error.
 * when parameter r < 0 the process gets killed and a error message is presented.
 */
#define CHECK(r) if(r < 0) {perror(""); fprintf(stderr, "file: %s line: %d.\n", __FILE__, __LINE__); exit(EXIT_VALUE_ERROR);}

/*
 * Just like the CHECK macro above, but kills all child processes before exit.
 */
#define CHECK_SAFE(r) if(r < 0) {perror(""); fprintf(stderr, "file: %s line: %d.\nKilling processes..\n", __FILE__, __LINE__); killProcesses(); exit(EXIT_VALUE_ERROR); }

/*
 * printLine works just like printf, but it prints a NEWLINE_CHAR before and after the text.
 *
 * Will terminated program with value EXIT_VALUE_ERROR on fatal errors.
 */
void printLine(const char *string, ...);

/*
 * readLine reads _size_ number of characters from the given stream _stream_. The newline character
 * will be removed and replaced with a '\0' char, so _buffer_ will at maximum contain _size_ - 2 non-'\0' chars.
 * readLine returns 0 on success. If no newline char is present at the end of the read buffer, the function will return 1.
 *
 * Will terminated program with value EXIT_VALUE_ERROR on fatal errors.
 */
unsigned int readLine(char *buffer, unsigned int size, FILE *stream);

/*
 * A function that executes a program in a separate process by calling execvp.
 * Arguments to the program to execute should be provided by a NULL-terminated
 * array of NULL-terminated strings. The first string in the array is used to
 * identify which program to execute. The _mode_ parameter tells if the program
 * should be executed in background or foreground mode, which can be set by using
 * CHILD_FOREGROUND or CHILD_BACKGROUND. A foreground process will block until terminated,
 * and additional execution info will be printed.
 *
 * Will terminate program with value EXIT_VALUE_ERROR on fatal errors.
 */
void executeChild(char *args[], unsigned int mode);

/*
 * This function will split the specified string _command_ into the array _args_
 * containing all words separated by whitespaces. The whitespaces will not be retained
 * in the array read. If no whitespaces exists, the first item of _args_ array will equal
 * the command string. The args array will be NULL-terminated, and therefore the size of the args
 * array must be the max number of arguments to be read + 1. This is specified with the _size_ parameter.
 * If size is less than 2 the function will treat this as a fatal error.
 *
 * Will terminate program with value EXIT_VALUE_ERROR on fatal errors.
 */
void explode(char *args[], const unsigned int size, char *command);

/*
 * This function checks if an '&' char is present at the end of _args_ string.
 * If it is present 1 will be return, otherwise 0. Mode determines if the '&' char
 * should be removed from _args_ if found. The _mode_ parameter can either be
 * BACKGROUND_REMOVE_CHAR or BACKGROUND_KEEP_CHAR.
 *
 * Will terminate program with value EXIT_VALUE_ERROR on fatal errors.
 */
unsigned int isBackgroundRequested(char **args, unsigned int size, unsigned int mode);

/*
 * waitProcesses will read status changes of all processes until no more processes wishes to
 * reports status changes, or if there are no child processes present.
 * The function will return 0 if there are no more status changes to be reported, and 1 if there
 * are no child processes. The function will print information including the paramter string _text_
 * on child processes status changes.
 *
 * Will terminate program with value EXIT_VALUE_ERROR on fatal errors.
 */
unsigned int waitProcesses(char *text);

/*
 * The function will send a SIGTERM signal to the whole process group.
 */
void killProcesses();

#endif
