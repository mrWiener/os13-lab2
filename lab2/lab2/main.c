/*
 *  main.c
 *  os13-lab2
 *
 *  See main.h for documentation.
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

#include "main.h"

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
                CHECK_SAFE(gettimeofday(&preExecute, NULL));
                
                /* Execute the program with the executeChild function with foreground mode. */
                executeChild(args, CHILD_FOREGROUND);
                
                /* Get the current time for command executions statistics. */
                CHECK_SAFE(gettimeofday(&postExecute, NULL));
                
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
            CHECK_SAFE(-1);
        }
    }
    
    /* Return the returnStatus. */
    return readStatus;
}

void signal_handler(int sig) {

    /* Check which signal was sent. */
    if(sig == SIGINT) {
        /* Interrupt program signal was sent. Probably because of Ctrl-C was pressed in shell. */

        /* The child processes of the shell have also received this signal, see if there are status reports. Do not exit the main shell program. */
        waitProcesses("SIGINT");
    } else if(sig == SIGTERM) {
        /* Termination signal was sent to program. */

        struct timeval startTime;      /* Structure to hold time info about the time when started counting. */
        struct timeval currentTime;    /* Structure to hold time info about the current time. */
        unsigned int elapsed = 0;      /* The number of seconds elapsed. */

        /* Get the current time. */
        CHECK(gettimeofday(&startTime, NULL));

        /* Loop until waitProcesses indicate that there are no active child processes. */
        while(waitProcesses("Exiting") != 1) {

            /* Get the current time. */
            CHECK_SAFE(gettimeofday(&currentTime, NULL));

            /* Calculate the number of seconds the program have been waiting for child processes to exit gracefully. */
            elapsed = currentTime.tv_sec - startTime.tv_sec;

            if(elapsed >= WAIT_CHILD_TERMINATION_LIMIT) {
                /* The time have exceeded the limit, time to get dirty with the child processes. */

                printLine("Forcing child processes to exit...");

                /* 
                 * Send SIGKILL signal to all processes in the same process group of parent (including parent). 
                 * This will force all processes to terminate, since SIGKILL cannot be ignored by processes.
                 */
                CHECK(kill(0, SIGKILL));
            }
        }

        /* Exit with success value. */
        exit(EXIT_VALUE_SUCCESS);
    }
 }

void registerSignalToHandle(int sig) {

    /* Register signal_handler to handle the signal paramter. */
    if(signal(sig, signal_handler) == SIG_ERR) {
        /* Failed to register signal handler. */

        /* Force an error. */
        CHECK(-1);
    }
}

int main(int argc, const char * argv[]) {

    /* Register program to handle SIGINT and SIGTERM signals. */
    registerSignalToHandle(SIGINT);
    registerSignalToHandle(SIGTERM);

    /* Prompt the user for input. */
    printf(PROMPT_TEXT);
    
    /* Read and execute commands until the exit command terminates the program, or an error occured. */
    while (1) {
        
        /* Read and execute commands. */
        if(readAndExecute() == 0) {
            /* Read and executed command successfully. */
            
            waitProcesses("Background");

            /* Prompt the user for input. */
            printf(PROMPT_TEXT);
        }
    }
    
    /* Kill all child processes before exiting. */
    killProcesses();

    /* Terminate process normally. */
    return EXIT_VALUE_SUCCESS;
}
