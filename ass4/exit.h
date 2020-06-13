#ifndef EXIT_H
#define EXIT_H

#include <stdio.h>
#include <stdlib.h>

/* Definitions */

#define NUM_CONTROL_ERRS 4    // Number of errors with are for controller

/* Types */

/*
    ExitCode:   Enum with all the exit codes specified in spec sheet
*/
typedef enum ExitCode {
    GOOD = 0,                 // Program completed with no problems
    BAD_CONTROL_ARGS = 1,     // Control2310 was inputted invalid num of args
    BAD_CHARS = 2,            // The id put into control2310 wasn't valid
    BAD_PORT_NUMBER = 3,      // The port inputted was  > 65535 or < 0
    BAD_PORT = 4,             // Valid port value, but couldn't connect to it
    BAD_ROC_ARGS = 5,         // Incorrect number of args into roc program
    BAD_MAPPER_PORT = 6,      // Port was not a valid port number (or -)
    MAPPER_REQUIRED = 7,      // Mapper wasn't provided into roc when required
    INVALID_MAPPER = 8,       // Couldn't connect to the mapper provided 
    BAD_MAP_ENTRIES = 9,      // No map entry for at least one destination
    BAD_MAP_CONNECTIONS = 10  // Failed to connect to at least one destination
} ExitCode;

/* Functions */

void err_msg(ExitCode type);

#endif