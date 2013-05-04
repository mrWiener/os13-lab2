/*
 *  main.h
 *  os13-lab2
 *
 *  TODO: Write about the program.
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

/* Include dependencies. */

#include <signal.h>     /* Needed for handling signals. */

#include "utils.h"      /* Needed for helper functions and macros. */
#include "commands.h"   /* Needed for handling built in commands. */

/* Define constants to be used in the program. */

#define WAIT_CHILD_TERMINATION_LIMIT 5	/* The program will wait maximum 5 seconds for child processes to exit gracefully before forcing them. */

#define PROMPT_TEXT     "> "    				/* The text to display when waiting for user command. */

 /*
 * This function reads a command from stdin of length COMMAND_MAX_LENGTH (newline and '\0' excluded).
 * Will try to execute the command by calling the function executeCommand (built-in commands), and if it fails
 * the function will execute the command by calling executeChild function. If an external program is executed,
 * the function will print the number of milliseconds the process was alive.
 *
 * The function returns 0 on success, 1 if too many characters was entered and -1 on error.
 *
 * Will terminated program with value EXIT_VALUE_ERROR on fatal errors.
 */
int readAndExecute();

/* 
 * TODO:
 */
void signal_handler(int signal);

/*
 * Program main entry point.
 *
 * A infinite loop will run which calls readAndExecute functions. If a command have been executed,
 * waitpid will be polled without blocking to check for status changes of background processes. Information
 * will be printed if an background process have terminated. Lastly, and prompt defined by PROMPT_TEXT will be printed.
 * Only an error or exit command will break the loop.
 *
 * Will terminate program with value EXIT_VALUE_ERROR on fatal errors.
 */
int main(int argc, const char * argv[]);