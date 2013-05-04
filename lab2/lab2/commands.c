/*
 *  commands.c
 *  lab2
 *
 *  See commands.h for documentation.
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

#include "commands.h"

unsigned int executeCommand(char **args) {
    const char *command = args[0]; /* Variable to hold the command name. */
    
    /* Check if the command name (first element in args) is built it argument. */
    if(*command == '\0') {
        /* Empty command. */
        
        /* Do nothing. */
        return 0;
    } else if(strncmp(command, COMMAND_CD, strlen(COMMAND_CD)) == 0) {
        /* The command is the built in cd-command. */
        
        /* Call the command function. */
        command_cd(args);
    } else if (strncmp(command, COMMAND_EXIT, strlen(COMMAND_EXIT)) == 0) {
        /* The command is the built in exit-command. */
        
        /* Call the command function. */
        command_exit();
    } else {
        /* Command not found. */
        
        /* Return with value 1 to indicate this. */
        return 1;
    }
    
    /* Command found and executed. Return 0 to indicate success. */
    return 0;
}

void command_cd(char **args) {
    const char *path; /* The path that have been set to the working directory. */
    
    if(args[1] == '\0' || args[2] != '\0') {
        /* The arguments are malformatted.*/
        
        /* Tell the user how to use the command. */
        printLine("Please use the command as such: cd <path>");
    } else {
        /* Arguments are good. */
        
        /* Change the working directory to the given one in the second argument. */
        if(chdir(args[1]) != 0) {
            /* The call was unsuccessful. */
            
            if(errno == ENOENT) {
                /* No such file or directory. */
                
                /* Now instead change directory to path given by env HOME. */
                CHECK_SAFE(chdir(getenv(ENV_HOME)));
            
                /* Tell user about this. */
                printLine("No such directory '%s'", args[1]);
                
                /* Set the path to the HOME path. */
                path = getenv(ENV_HOME);
            } else {
                /* An error which is not handled occured. */
                
                /* Force an error. */
                CHECK_SAFE(-1);
            }
        } else {
            /* The call was successful. */
            
            /* Set the path to the argument. */
            path = args[1];
        }
        
        /* Working directory have successfuly been changed. Let user know.*/
        printLine("Working directory changed to '%s'", path);
    }
}

void command_exit() {
    
    /* Kill all child processes before exiting. */
    killProcesses();

    /* Terminate the program with EXIT_VALUE_SUCCESS. */
    exit(EXIT_SUCCESS);
}
