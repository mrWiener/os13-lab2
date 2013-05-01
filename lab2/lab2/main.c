/*
 *  main.c
 *  os13-lab2
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

#include <stdio.h>      /* Needed for read and print functions. */
#include <stdlib.h>     /* Needed for exit function. */
#include <string.h>     /* Needed for string manipulations functions. */
#include <sys/time.h>   /* Needed for time functions. */
#include <unistd.h>     /* Needed for the chdir function. */
#include <errno.h>      /* Needed for error handling. */

#include "utils.h"      /* Needed for helper functions and macros. */

#define COMMAND_MAX_LENGH       70      /* Max number of characters an user specified command can be. */
#define ARGUMENTS_MAX_LENGTH    5       /* Max number of arguments that can be specified with commands. */

#define PROMPT_TEXT             "> "    /* The text to display when waiting for user command. */

#define COMMAND_CD              "cd"    /* The command to change working directory. */
#define COMMAND_EXIT            "exit"  /* The command to exit the program. */


/* Declaration of functions. */

void executeChild(char *args[]);
void parseArguments(char *args[], const unsigned int size, char *command);
void readAndExecute(const unsigned int cmdMaxSize);

/*
 * A function read a command from stdin of length cmdMaxSize (newline excluded).
 * Will execute the command by calling executeChild function. If an external program is executed, 
 * the function will print the number of milliseconds the process were alive.
 */
void readAndExecute(const unsigned int cmdMaxSize) {
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
        if(strncmp(args[0], COMMAND_CD, cmdMaxSize) == 0) {
            /* The command is the built in cd-command. */
            
            if(args[1] == '\0' || args[2] != '\0') {
                /* The arguments are malformatted.*/
                
                /* Tell the user how to use the command. */
                printLine("Please use the command as such: cd <path>");
            } else {
                /* Arguments are good. */
                
                /* Change the working directory to the given one in the second argument. */
                if(chdir(args[1]) == -1) {
                    /* The call was unsuccessful. */
                    
                    if(errno == ENOENT) {
                        /* No such file or directory. */
                        
                        /* Tell user about this. */
                        printLine("No such directory '%s'", args[1]);
                    } else {
                        /* An error which is not handled occured. */
                        
                        /* Force an error. */
                        CHECK(-1);
                    }
                }
                
                /* Working directory have successfuly been changed. Let user know.*/
                printLine("Working directory changed to '%s'", args[1]);
            }
        } else if (strncmp(input, COMMAND_EXIT, cmdMaxSize) == 0) {
            /* The command is the built in exit-command. */
            
            /* Just terminate the program with EXIT_VALUE_SUCCESS. */
            exit(EXIT_VALUE_SUCCESS);
        } else {
            /* The command is not a built in command. The command should be executed exeternally. */
            
            struct timeval preExecute;      /* Structure to hold time info from pre execution of the command. */
            struct timeval postExecute;     /* Structure to hold time info from post execution of the command. */
            unsigned int elapsed;           /* The number of milliseconds needed for executing the command. */
            
            /* Get the current time for command execution statistics. */
            CHECK(gettimeofday(&preExecute, NULL));
            
            /* Execute the program with the executeChild function. */
            executeChild(args);
            
            /* Get the current time for command executions statistics. */
            CHECK(gettimeofday(&postExecute, NULL));
            
            /* Return the elapsed time. */
            elapsed = postExecute.tv_usec - preExecute.tv_usec;
            
            printLine("Command '%s' executed in %i milliseconds.", args[0], elapsed);
        }
   
    } else if(readStatus == 1) {
        /* Too many characters were read. */
        
        /* Tell user to enter less number of characters. */
        printLine("The character limit of a command is %i", COMMAND_MAX_LENGH);
    } else if(readStatus == -1) {
        /* An error have occured. */
        
        /* Force an error. */
        CHECK(-1);
    }
}

/*
 * Program main entry point.
 */
int main(int argc, const char * argv[]) {
    
    /* Read and execute commands until the exit command terminates the program, or an error occured. */
    while (1) {
        
        /* Prompt the user for input. */
        printf(PROMPT_TEXT);
        
        /* Start the command reading and executions. */
        readAndExecute(COMMAND_MAX_LENGH);
    }
    
    /* Terminate process normally. */
    return EXIT_VALUE_SUCCESS;
}