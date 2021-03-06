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
    CHECK_SAFE(putchar(NEWLINE_CHAR));
    
    /* Call print the string with vprintf. */
    CHECK_SAFE(vprintf(string, ap));
    
    /* Print the last newline. */
    CHECK_SAFE(putchar(NEWLINE_CHAR));
    
    /* Cleanup the va_list. */
    va_end(ap);
}

unsigned int readLine(char *buffer, unsigned int size, FILE *stream) {
    int c;          /* A temporary character to be used when clearing the stdin stream. */
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
            CHECK_SAFE(-1);
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
    
   /* 
    * If the program came this far, no newline was found in the line. This means the limit was hit, and a whole line
    * was not read. This means that there are unread data still on stdin, which will be read next time readLine is called.
    * This is an unwanted behavior, so the program now needs to clear the stdin.
    */

    /* Keep reading a character from stdin until a newline is read or end of file is retrieved. */
    while((c = fgetc(stdin)) != '\n' && c != EOF);

    if(c == EOF) {
        /* Either end of file was encountered, or an error encountered. */

        if(ferror(stdin) != 0) {
            /* An error occured. */
            
            /* Force an error. */
            CHECK_SAFE(-1);
        }
    }
    
    /* Return 1 to indicate this situation. */
    return 1;
}

void executeChild(char *args[], unsigned int mode) {
    pid_t pid;      /* Variable to hold child process id. */
    int fd[2];      /* Array of file descriptors to be used for piping. */
    
    /* Create a child process. */
    CHECK_SAFE((pid = fork()));
    
    /* Setup pipe for printenv-child. */
    CHECK_SAFE(pipe(fd));
    
    if(pid == 0) {
        /* Child area. */

        if(mode == CHILD_BACKGROUND) {	
            /* The child is gonna be executed as a background process. No output or readig can be done by this child. */
            
            /* Duplicate STANDARD_OUTPUT to child output pipe. When exec prints to STANDARD_OUTPUT it will instead print to child output pipe. */
            CHECK(dup2(fd[PIPE_OUT], STANDARD_OUTPUT));
            
            /* Duplicate STANDARD_ERROR to child output pipe. When exec prints to STANDARD_ERROR it will instead print to child output pipe. */
            CHECK(dup2(fd[PIPE_OUT], STANDARD_ERROR));
            
            /* Duplicate STANDARD_INPUT to child input pipe. When exec reads from STANDARD_INPUT it will instead read from child input pipe. */
            CHECK(dup2(fd[PIPE_IN], STANDARD_INPUT));
        } else if(mode == CHILD_FOREGROUND) {
            /* The child is gonna be executed as a foreground process. Do not redirect any output or input. */
            
            /* Close output pipe. */
            CHECK(close(fd[PIPE_OUT]));
        
            /* Close input pipe. */
            CHECK(close(fd[PIPE_IN]));
        }

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
        
        /* If child is executed in background mode, the parent should ignore the output and input.
         * If the child is executed in foreground mode, the child process will handle the output and input.
         * So close both file descriptors.
         */
        
        /* Close output pipe. */
        CHECK_SAFE(close(fd[PIPE_OUT]));
        
        /* Close input pipe. */
        CHECK_SAFE(close(fd[PIPE_IN]));
        
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
                    CHECK_SAFE(-1);
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
                    CHECK_SAFE(-1);
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
            CHECK_SAFE(-1);
        }
    }
}

void explode(char *args[], const unsigned int size, char *command) {
    unsigned int i;              /* Used in the for-loop. */
    const char *separator = " "; /* The separator to be used when calling strtok. */
    
    if(size < 2) {
        /* The size of the args array is not allowed to be less than 2. */
        
        /* Force an error. */
        CHECK_SAFE(-1);
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
                    CHECK_SAFE(-1);
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

unsigned int waitProcesses(char *text) {
    int status; /* Variable to hold child termination status. */
    pid_t pid;  /* Variable to hold the terminated child process id. */

    /* Keep checking if any childs have terminated until waitpid reports that no child processes have a status to report. */
    while((pid = waitpid(-1, &status, WNOHANG)) != 0) {
        /* A child process have changed status. */

        if(pid == -1) {
            /* An error occured. */
            
            /* Check the error. */
            if(errno == ECHILD) {
                /* No child processes active. */
                
                /* Return 1 to indicate that there are no child processes. */
                return 1;
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
                    printLine("%s: process '%i' terminated normally.", text, pid);
                } else {
                    /* Process exited with error. */
                    
                    /* Let user know. */
                    printLine("%s: process '%i' terminated with error '%i'", text, pid, exit_value);
                }
            } else if(WIFSIGNALED(status)) {
                /* Process terminated due to receipt of a signal. */
                
                /* Retrieve the signal value. */
                int signal_value = WTERMSIG(status);
                
                /* Let user know the signal. */
                printLine("%s: process '%i' killed by signal '%i'", text, pid, signal_value);
            } else {
                /* Process terminated in an unknown way. This should not happen. */
                
                /* Force error. */
                CHECK(-1);
            }
        }
    }

    /* Return 0 to indicate that there are no processes that want to report status change. */
    return 0;
}

void killProcesses() {

    /* Send SIGTERM signal to all processes in the same process group of parent (including parent). */
    CHECK(kill(0, SIGTERM));
}