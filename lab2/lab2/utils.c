/*
 *  utils.c
 *  lab2
 *
 *  See utils.h for documentation.
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

#include "utils.h"

void printLine(const char *string, ...) {
    va_list ap;     /* List to hold the arguments. */
    
    /* Initialize the argument list. */
    va_start(ap, string);
    
    /* Print the first newline. */
    CHECK(putchar(NEWLINE_CHAR));
    
    /* Call print the string with vprintf. */
    CHECK(vprintf(string, ap));
    
    /* Print the last newline. */
    CHECK(putchar(NEWLINE_CHAR));
    
    /* Cleanup the va_list. */
    va_end(ap);
}

unsigned int readLine(char *buffer, unsigned int size, FILE *stream) {
    int i;          /* An integer that will be used for scanning the input array. */
    char *result;   /* A pointer to be used when calling fgets method. The pointer will hold the return value. */
    
    /* Read from stdin into input array. */
    result = fgets(buffer, size, stream);
    
    /* Check read status by controlling the result pointer. */
    if(result == NULL) {
        /* Nothing was read. Check if an error occured. */
        
        if(ferror(stdin) != 0) {
            /* An error occured. */
            
            /* Force an error. */
            CHECK(-1);
        }
    }
    
    /* Remove newline character from the input buffer. Start from next last character since last is '\0'. */
    for(i = size-2; i >= 0; i--) {
        if(buffer[i] == '\n') {
            /* The value of the character in position i in the input array is an newline character. */
            
            /* Remove the '\n' charcter by replacing it with '\0'. */
            buffer[i] = '\0';
            
            /* Since there can only exist one newline character, the loop can be stopped. Exit with success status. */
            return 0;
        }
    }
    
    /* If the program came this far, no newline was found in the line. This means the limit was hit, and a whole line
     * was not read. return 1 to indicate this situation. */
    return 1;
}

void executeChild(char *args[], unsigned int mode) {
    pid_t pid;      /* Variable to hold child process id. */
    
    /* Create a child process. */
    CHECK((pid = fork()));
    
    if(pid == 0) {
        /* Child area. */
        
        /* Execute program specified as the first string in the arguments array */
        if(execvp(args[0], args) != 0) {
            /* The execution failed. */
            
            /* Write error messag to user. */
            perror("\nCommand failed");
            
            /* Exit with failure status. */
            exit(errno);
        }
    } else {
        /* Parent area. */
        
        if(mode == CHILD_FOREGROUND) {
            /* Child is executing in foreground. */
            
            int status; /* Integer to be used for checking child status. */
            
            /* Print info to user. */
            printf("Foreground: executing '%s', with pid '%i'\n", args[0], pid);
        
        wait: /* A label to be used when waitpid encounters the EINTR error, which basically means 'try again'. */
            
            /* Block until the child changes status. */
            pid = waitpid(pid, &status, 0);
            
            if(pid == -1) {
                /* An error occured. */
                
                /* Check the error. */
                if(errno == EINTR) {
                    /* Interrupted system call. */
                    
                    /* That's okay. Just try again. */
                    goto wait;
                } else {
                    /* Unhandled error. */
                    
                    /* Force program exit. */
                    CHECK(-1);
                }
            } else if(pid != 0) {
                /* The process have reported a status change. */
                
                /* Check termination status. */
                if(WIFEXITED(status)) {
                    /* Child process did terminate by calling exit. */
                    
                    /* Check the exit value of the status. */
                    int exit_value = WEXITSTATUS(status);
                    
                    if(exit_value == 0) {
                        /* The process exited normally. */
                        
                        /* Let user know. */
                        printLine("Foreground: process '%i' terminated normally.", pid);
                    } else {
                        /* Process exited with error. */
                        
                        /* Let user know. */
                        printLine("Foreground: process '%i' terminated with error '%i'", pid, exit_value);
                    }
                } else if(WIFSIGNALED(status)) {
                    /* Process terminated due to receipt of a signal. */
                    
                    /* Retrieve the signal value. */
                    int signal_value = WTERMSIG(status);
                    
                    /* Let user know the signal. */
                    printLine("Foreground: process '%i' killed by signal '%i'", pid, signal_value);
                } else {
                    /* Process terminated in an unknown way. This should not happen. */
                    
                    /* Force error. */
                    CHECK(-1);
                }
            }
        } else if(mode == CHILD_BACKGROUND){
            /* Child is executing in background. */
            
            /* Tell user that the command is being executed. */
            printLine("Background: executing '%s', with pid '%i'", args[0], pid);
            
            /* Do nothing more since the status changes of childs will be handles externally. */
        } else {
            /* Uknown mode passed to function. */
            
            /* Force an error. */
            CHECK(-1);
        }
    }
}

void explode(char *args[], const unsigned int size, char *command) {
    unsigned int i;              /* Used in the for-loop. */
    const char *separator = " "; /* The separator to be used when calling strtok. */
    
    if(size < 2) {
        /* The size of the args array is not allowed to be less than 2. */
        
        /* Force an error. */
        CHECK(-1);
    }
    
    /* Read the first chunk of chars separated by whitespace. */
    args[0] = strtok(command, separator);
    
    if(args[0] == NULL) {
        /* No whitespaces exists. */
        
        /* Put the command string as the only argument. */
        args[0] = command;
        
        /* The array must be NULL-terminated, so set the next item to NULL. */
        args[1] = '\0';
        
        /* Stop the function since no further search is necessary. */
        return;
    }
    
    /* Perform a loop until no more whitespaces are found. Only read size-1 number of arguments. */
    for(i = 1; i < size-1; i++) {
        
        /* Read the next chunk of chars separated by a whitespace into the next element of the array. */
        args[i] = strtok(NULL, separator);
        
        if(args[i] == NULL) {
            /* There are no more whitespaces. */
            
            /* The current element of the array is already set to NULL, so just stop the loop. */
            break;
        }
    }
}

unsigned int isBackgroundRequested(char **args, unsigned int size, unsigned int mode) {
    unsigned int i;     /* Used in the for-loop. */
    
    /* Loop through args to find '&' character. */
    for(i = 0; i < size; i++) {
        
        /* Check if end of args is found. */
        if(args[i] == '\0') {
            /* char '\0' encountered, which means end of args. */
            
            if(*args[i-1] == BACKGROUND_CHAR) {
                /* The char before the '\0' char is the '&', which means the command was ended with the background char. */
                
                if(mode == BACKGROUND_REMOVE_CHAR) {
                    /* The char '&' should be removed. */
                    
                    /* Remove it by replacing the char with '/0'. */
                    args[i-1] = '\0';
                } else if(mode == BACKGROUND_KEEP_CHAR) {
                    /* The char should not be removed. */
                    
                    /* Do nothing. */
                } else {
                    /* Unknown mode sent to function. */
                    
                    /* Force an error. */
                    CHECK(-1);
                }
                
                /* The args parameter is ended with an '&' char, return 1 to indicate this. */
                return 1;
            } else {
                /* The last char is not an '&' char, and the command was therefore not ended by '&'. */

                /* return 0 to indicate this. */
                return 0;
            }
        }
    }
    
    /* The '&' char is not present in the command. */
    return 0;
}