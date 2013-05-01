/*
 *  utils.c
 *  lab2
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

int readLine(char *buffer, unsigned int size, FILE *stream) {
    unsigned int loop;      /* An integer that will be used for scanning the input array. */
    char *result;           /* A pointer to be used when calling fgets method. The pointer will hold the return value. */
    
    /* Read from stdin into input array. */
    result = fgets(buffer, size, stream);
    
    /* Check read status by controlling the result pointer. */
    if(result == NULL) {
        /* Nothing was read. Check if an error occured. */
        
        if(ferror(stdin) != 0) {
            /* An error occured. */
            
            /* Return -1 to indicate error. */
            return -1;
        }
    }
    
    /* Remove newline character from the input buffer. Start from next last character since last is '\0'. */
    for(loop = size-2; loop > 0; loop--) {
        if(buffer[loop] == '\n') {
            /* The value of the character in position i in the input array is an newline character. */
            
            /* Remove the '\n' charcter by replacing it with '\0'. */
            buffer[loop] = '\0';
            
            /* Since there can only exist one newline character, the loop can be stopped. Exit with success status. */
            return 0;
        }
    }
    
    /* If the program came this far, no newline was found in the line. This means the limit was hit, and a whole line
     * was not read. return 1 to indicate this situation. */
    return 1;
}

void executeChild(char *args[]) {
    pid_t pid;      /* Variable to hold child process id. */
    
    /* Create a child process. */
    CHECK((pid = fork()));
    
    if(pid == 0) {
        /* Child area. */
        
        /* Execute program specified as the first string in the arguments array */
        CHECK(execvp(args[0], args));
    } else {
        /* Parent area. */
        
        int status; /* Variable to hold child termination status. */
        
        /* Wait for child to terminate. This method call will be blocking since options is set to 0. */
        waitpid(pid, &status, 0);
        
        /* Check termination status. */
        if(!WIFEXITED(status)) {
            /* Child process did not terminate by calling exit. */
            
            /* Force error. */
            CHECK(-1);
        } else {
            /* Child process did exit normally. */
            
            /* Check the exit value of the status. */
            int exit_value = WEXITSTATUS(status);
            
            if(exit_value == 1) {
                /* The process exited with an error. */
                
                /* Exit program with exit value EXIT_VALUE_ERROR to indicate error. */
                exit(EXIT_VALUE_ERROR);
            }
        }
    }
}

void explode(char *args[], const unsigned int size, char *command) {
    unsigned int loop;              /* Used in the for-loop. */
    const char *separator = " ";    /* The separator to be used when calling strtok. */
    
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
    for(loop = 1; loop < size-1; loop++) {
        
        /* Read the next chunk of chars separated by a whitespace into the next element of the array. */
        args[loop] = strtok(NULL, separator);
        
        if(args[loop] == NULL) {
            /* There are no more whitespaces. */
            
            /* The current element of the array is already set to NULL, so just stop the loop. */
            break;
        }
    }
}