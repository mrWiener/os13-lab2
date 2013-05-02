/*
 *  main.c
 *  os13-lab2
 *
 *  TODO: Write about the program.
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

/* Include dependencies. */

#include <stdio.h>      /* Needed for read and print functions. */
#include <stdlib.h>     /* Needed for exit function. */
#include <string.h>     /* Needed for string manipulations functions. */
#include <sys/time.h>   /* Needed for time functions. */
#include <unistd.h>     /* Needed for the chdir function. */
#include <errno.h>      /* Needed for error handling. */

#include "utils.h"      /* Needed for helper functions and macros. */
#include "commands.h"   /* Needed for handling built in commands. */

/* Define constants to be used in the program. */

#define PROMPT_TEXT     "> "    /* The text to display when waiting for user command. */

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
int readAndExecute() {
    int readStatus;                     /* An variable to hold the return value of the readStatus function. */
    char input[COMMAND_MAX_LENGH+2];    /* Array to be used for reading, with 2 extra char for newline and the '\0' char. */
    
    /* Read a line of max COMMAND_MAX_LENGTH command chars from stdin. */
    readStatus = readLine(input, COMMAND_MAX_LENGH+2, stdin);
    
    /* Check the read line status */
    if(readStatus == 0) {
        /* Read was successful. */
        
        char *args[ARGUMENTS_MAX_LENGTH+2]; /* Array to hold all the arguments. 2 extra chars for command name and '\0' char. */
        
        /* Parse the command line by exploding command string. Args will then contain all arguments of the command. */
        explode(args, ARGUMENTS_MAX_LENGTH+2, input);
        
        /* Check for built-in commands. */
        if(executeCommand(args) != 0) {
            /* The command is not a built in command. The command should be executed externally. */
            
            struct timeval preExecute;      /* Structure to hold time info from pre execution of the command. */
            struct timeval postExecute;     /* Structure to hold time info from post execution of the command. */
            unsigned int elapsed;           /* The number of milliseconds needed for executing the command. */
            
            if(isBackgroundRequested(args, ARGUMENTS_MAX_LENGTH+2, BACKGROUND_REMOVE_CHAR) == 0) {
                /* Command should be executed in foreground. */
                
                /* Get the current time for command execution statistics. */
                CHECK(gettimeofday(&preExecute, NULL));
                
                /* Execute the program with the executeChild function with foreground mode. */
                executeChild(args, CHILD_FOREGROUND);
                
                /* Get the current time for command executions statistics. */
                CHECK(gettimeofday(&postExecute, NULL));
                
                /* Return the elapsed time. */
                elapsed = postExecute.tv_usec - preExecute.tv_usec;
                
                /* Print the statistics. */
                printf("Command '%s' executed in %i milliseconds.\n", args[0], elapsed);
            } else {
                /* Command should be executed in background. */
            
                /* Execute the program with the executeChild function with background mode. */
                executeChild(args, CHILD_BACKGROUND);
            }
        }
    } else if(readStatus == 1) {
        /* Too many characters were read. */
        
        /* Tell user to enter less number of characters. */
        printLine("The character limit of a command is %i", COMMAND_MAX_LENGH);
    } else if(readStatus == -1) {
        /* An error have occured. */
        
        /* Check the error. */
        if(errno == EINTR) {
            /* Interrupted system call. */
            
            /* Do not treat this like an error, just try again. */
        } else {
            /* Unhandled error. */
        
            /* Force an error. */
            CHECK(-1);
        }
    }
    
    return readStatus;
}

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
int main(int argc, const char * argv[]) {
    int status; /* Variable to hold child termination status. */
    pid_t pid;  /* Variable to hold the terminated child process id. */
    
    /* Prompt the user for input. */
    printf(PROMPT_TEXT);
    
    /* Read and execute commands until the exit command terminates the program, or an error occured. */
    while (1) {
        
        /* Read and execute commands. */
        if(readAndExecute() == 0) {
            /* Read and executed command successfully. */
            
            /* Check if any childs have terminated. */
            pid = waitpid(-1, &status, WNOHANG);
            
            if(pid == -1) {
                /* An error occured. */
                
                /* Check the error. */
                if(errno == ECHILD) {
                    /* No child processes active. */
                    
                    /* That's okay. Keep waiting for processes. */
                } else {
                    /* Unhandled error. */
                    
                    /* Force program exit. */
                    CHECK(-1);
                }
            } else if(pid != 0) {
                /* A process have reported a status change. */
                
                /* Check termination status. */
                if(WIFEXITED(status)) {
                    /* Child process did terminate by calling exit. */
                    
                    /* Check the exit value of the status. */
                    int exit_value = WEXITSTATUS(status);
                    
                    if(exit_value == 0) {
                        /* The process exited normally. */
                        
                        /* Let user know. */
                        printLine("Background: process '%i' terminated normally.", pid);
                    } else {
                        /* Process exited with error. */
                        
                        /* Let user know. */
                        printLine("Background: process '%i' terminated with error '%i'", pid, exit_value);
                    }
                } else if(WIFSIGNALED(status)) {
                    /* Process terminated due to receipt of a signal. */
                    
                    /* Retrieve the signal value. */
                    int signal_value = WTERMSIG(status);
                    
                    /* Let user know the signal. */
                    printLine("Background: process '%i' killed by signal '%i'", pid, signal_value);
                } else {
                    /* Process terminated in an unknown way. This should not happen. */
                    
                    /* Force error. */
                    CHECK(-1);
                }
            }
            
            /* Prompt the user for input. */
            printf(PROMPT_TEXT);
        }
    }
    
    /* Terminate process normally. */
    return EXIT_VALUE_SUCCESS;
}