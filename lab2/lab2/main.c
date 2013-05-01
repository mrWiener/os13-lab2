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

#define COMMAND_MAX_LENGH       70      /* Max number of characters an user specified command can be. */
#define ARGUMENTS_MAX_LENGTH    5       /* Max number of arguments that can be specified with commands. */

#define EXIT_VALUE_SUCCESS      0       /* The value which the program will exit with on success. */
#define EXIT_VALUE_ERROR        1       /* The value which the program will exit with on error. */

#define PROMPT_TEXT             "> "    /* The text to display when waiting for user command. */

#define COMMAND_CD              "cd"    /* The command to change working directory. */
#define COMMAND_EXIT            "exit"  /* The command to exit the program. */

/*
 * Macros to print better error messages and exit the process on error.
 * when parameter r == -1 the process gets killed and a error message is presented.
 */
#define CHECK(r) if(r == -1) {perror(""); fprintf(stderr, "line: %d.\n", __LINE__); exit(EXIT_VALUE_ERROR);}

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
    unsigned int loop;                  /* An integer that will be used for scanning the input array. */
    char *result;                       /* A pointer to be used when calling fgets method. The pointer will hold the return value. */
    char *args[ARGUMENTS_MAX_LENGTH+2]; /* Array to hold all the arguments. 2 extra chars for command name and '\0' char. */
    char input[COMMAND_MAX_LENGH+2];    /* Array to be used for reading, with 2 extra char for newline and the '\0' char. */
    
    /* Read from stdin into input array. */
    result = fgets(input, COMMAND_MAX_LENGH+1, stdin);
    
    /* Check read status by controlling the result pointer. */
    if(result == NULL) {
        /* Nothing was read. Check if an error occured. */
        
        if(ferror(stdin) != 0) {
            /* An error occured. */
            
            /* Exit program with EXIT_VALUE_ERROR. */
            exit(EXIT_VALUE_ERROR);
        }
    }
    
    /* Remove newline character from the input buffer. Start from next last character since last is '\0'. */
    for(loop = COMMAND_MAX_LENGH-2; loop > 0; loop--) {
        if(input[loop] == '\n') {
            /* The value of the character in position i in the input array is an newline character. */
            
            /* Remove the '\n' charcter by replacing it with '\0'. */
            input[loop] = '\0';
            
            /* Since there can only exist one newline character, the loop can be stopped. */
            break;
        }
    }
    
    /* Parse the command line. Args will then contain all arguments of the command. */
    parseArguments(args, ARGUMENTS_MAX_LENGTH+2, input);
    
    
    
    /* Check for built-in commands. */
    if(strncmp(args[0], COMMAND_CD, cmdMaxSize) == 0) {
        /* The command is the built in cd-command. */
        
        
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
        
        printf("\nCommand '%s' executed in %i milliseconds. \n", args[0], elapsed);
    }
}

/*
 * A function that executes a program in a separate process by calling execvp.
 * Arguments to the program to execute should be provided by a NULL-terminated
 * array of NULL-terminated strings. The first string in the array is used to
 * identify which program to execute.
 */
void executeChild(char *args[]) {
    pid_t pid;  /* Variable to hold child process id. */
    
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

/*
 * This function will split the specified string _command_ into the array _args_
 * containing all words separated by whitespaces. The whitespaces will not be retained
 * in the array read. If no whitespaces exists, the first item of _args_ array will equal
 * the command string. The args array will be NULL-terminated, and therefore the size of the args
 * array must be the max number of arguments to be read + 1. This is specified with the _size_ parameter.
 * If any error is encountered, the function will exit the program will value EXIT_VALUE_ERROR.
 * If size is less than 1 will treat this as an error.
 */
void parseArguments(char *args[], const unsigned int size, char *command) {
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