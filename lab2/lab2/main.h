/*
 *  main.h
 *  os13-lab2
 *  
 *  This program is a simple shell. It will parse commands and try to execute them in foreground or background processes.
 *  There are two built-in commands, exit and cd. The cd command should be used as so: cd <path> where path is the path
 *  to which the working directory should be set. If the path is invalid then the path is set to HOME path in environment variable.
 *  The exit command exits the shell. To run a command in background a '&' character should be present last in the command string.
 *  Comands can be of max length COMMAND_MAX_LENGTH and the number of arguments to a command can max be ARGUMENTS_MAX_SIZE.
 *  The shell will terminate with status EXIT_VALUE_ERROR on fatal errors, else the user will be prompted with information
 *  about less fatal errors. If Ctrl-C (SIGINT) is sent to the shell, all of its child processes will be terminated.
 *	The shell program will terminate all child processes before exiting, except on fatal errors.
 * 
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

/* Include dependencies. */

#include <signal.h>     /* Needed for handling signals. */

#define COMMAND_MAX_LENGTH	10

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
 * Will terminated program with value EXIT_VALUE_ERROR on fatal errors.
 */
void readAndExecute();

/* 
 * Signal handler function to be called when a registered signa is received by the program.
 * This function now handles SIGTERM and SIGINT signals. When SIGINT is received, the function
 * calls waitProcesses and then ignores the signal. When SIGTERM is received, the function
 * waits for child processes to terminate gracefully for WAIT_CHILD_TERMINATION_LIMIT number of seconds.
 * If the limit is exceeded, then a SIGKILL signal is sent to all processes within the process group,
 * which kills the program and all of its processes.
 * 
 * Will terminate program on fatal errors.
 */
void signalHandler(int sig);

/*
 * Registers program to handle signal _sig_ by the function signalHandler.
 * 
 * Will terminate program with value EXIT_VALUE_ERROR on fatal errors.
 */
void registerSignalToHandle(int sig);

/*
 * Program main entry point.
 *
 * A infinite loop will run which calls readAndExecute functions. If a command have been executed,
 * waitpid will be polled without blocking to check for status changes of background processes. Information
 * will be printed if an background process have terminated. Lastly, and prompt defined by PROMPT_TEXT will be printed.
 * Only an error or exit command will break the loop.
 * 
 * Will terminated program with value EXIT_VALUE_ERROR on fatal errors.
 */
int main(int argc, const char * argv[]);
