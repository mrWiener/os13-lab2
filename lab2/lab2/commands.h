/*
 *  commands.h
 *  lab2
 *
 *  The commands.h file is responsible for the built-in commands of the shell program.
 *  The file declares the available commands, and functions to execute them.
 *  Also and command parser is declared, which tries to execute the given command if there
 *  are any matching built-in commands.
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

#ifndef lab2_commands_h
#define lab2_commands_h

#include "utils.h"      /* Needed for string handling functions and error handling. */

#define COMMAND_CD                  "cd"    /* The command to change working directory. */
#define COMMAND_EXIT                "exit"  /* The command to exit the program. */

/* Define COMMAND_MAX_LENGTH if not already defined. */
#ifndef COMMAND_MAX_LENGTH
    #define COMMAND_MAX_LENGTH       70      /* Max number of characters an user specified command can be. */
#endif

/* Define ARGUMENTS_MAX_LENGTH if not already defined. */
#ifndef ARGUMENTS_MAX_LENGTH
    #define ARGUMENTS_MAX_LENGTH    5       /* Max number of arguments that can be specified with commands. */
#endif

/* 
 * The function parses the given _args_ to see if the first argument is matching any built-in commands.
 * If so, the function will execute the command by calling the command function.
 * The function returns 0 on success or if _args_ is empty. Otherwise 1 will be returned to indicate that 
 * no built-in command was executed.
 */
unsigned int executeCommand(char **args);


/*
 * This command changed the working directory of the program. It accepts one extra argument which is the path
 * which should be the new working directory. If the path does not exist, the directory specified in HOME environment
 * variable will be changed to.
 *
 * Will terminate program with value EXIT_VALUE_ERROR on fatal errors.
 */
void command_cd(char **args);

/*
 * This function exits the program by calling exit function with EXIT_VALUE_SUCCESS. It also makes sure that all child processes are killed
 * before exiting.
 */
void command_exit();

#endif
