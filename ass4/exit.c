#include "exit.h"

/*
    summary:            Prints error message corresponding to ExitCode inputted
                        Terminates calling program with specified ExitCode.

    type:               The ExitCode type to print the message for
                        and to exit the program with
*/
void err_msg(ExitCode type)
{
    char *msg = ""; // set to blank in-case ExitCode not caught in switch

    switch (type) {
        case GOOD:
            exit(GOOD); // no need to print message if it exited as "GOOD"
            break;
        case BAD_CONTROL_ARGS:
            msg = "Usage: control2310 id info [mapper]";
            break;
        case BAD_CHARS:
            msg = "Invalid char in parameter";
            break;  
        case BAD_PORT_NUMBER:
            msg = "Invalid port";
            break;
        case BAD_PORT:
            msg = "Can not connect to map";
            break;
        case BAD_ROC_ARGS:
            msg = "Usage: roc2310 id mapper {airports}";
            break;
        case BAD_MAPPER_PORT:
            msg = "Invalid mapper port";
            break;
        case MAPPER_REQUIRED:
            msg = "Mapper required";
            break;
        case INVALID_MAPPER:
            msg = "Failed to connect to mapper";
            break;
        case BAD_MAP_ENTRIES:
            msg = "No map entry for destination";
            break;
        case BAD_MAP_CONNECTIONS:
            msg = "Failed to connect to at least one destination";
            break;
    }

    // print error message
    fprintf(stderr, "%s\n", msg);
    fflush(stderr);

    // So programs exit with the right ExitCode
    if (type > NUM_CONTROL_ERRS) {
        exit(type - NUM_CONTROL_ERRS); 
    } else {
        exit(type);
    }
}