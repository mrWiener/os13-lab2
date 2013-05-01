/*
 *  commands.h
 *  lab2
 *
 *  Created by Lucas Wiener & Mathias Lindblom.
 *  Copyright (c) 2013 os13. All rights reserved.
 */

#ifndef lab2_commands_h
#define lab2_commands_h

#include "utils.h"
#include <errno.h>      /* Needed for error handling. */

#define COMMAND_CD                  "cd"    /* The command to change working directory. */
#define COMMAND_EXIT                "exit"  /* The command to exit the program. */

/* Define COMMAND_MAX_LENGTH if not already defined. */
#ifndef COMMAND_MAX_LENGTH
    #define COMMAND_MAX_LENGH       70      /* Max number of characters an user specified command can be. */
#endif

/* Define ARGUMENTS_MAX_LENGTH if not already defined. */
#ifndef ARGUMENTS_MAX_LENGTH
    #define ARGUMENTS_MAX_LENGTH    5       /* Max number of arguments that can be specified with commands. */
#endif

unsigned int executeCommand(char **args);

#endif