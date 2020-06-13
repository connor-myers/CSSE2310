#include "errs.h"

/**
* Prints error message associtated with inputted error code to stderr
* @param type the error code to print the message for to stderr
*/
void err_msg(Err type) {
    char* msg = "";
    switch (type) {
        case GOOD:
            exit(GOOD); // no need to print message
            break; // not needed; for A E S T H E T I C
        case BAD_DEALER_ARGS:
            msg = "Usage: 2310dealer deck path p1 {p2}";
            break;
        case BAD_DEALER_PATH:
            msg = "Error reading path";
            break;  
        case BAD_DECK:
            msg = "Error reading deck";
            break;
        case BAD_PROCESS:
            msg = "Error starting process";
            break;
        case DEALER_COM_ERROR:
            msg = "Communications error";
            break;
        case BAD_PLAYER_ARGS:
            msg = "Usage: player pcount ID";
            break;
        case BAD_PLAYER_COUNT:
            msg = "Invalid player count";
            break;
        case BAD_PLAYER_ID:
            msg = "Invalid ID";
            break;
        case BAD_PLAYER_PATH:
            msg = "Invalid path";
            break;
        case EARLY_GAME_OVER:
            msg = "Early game over";
            break;
        case PLAYER_COM_ERROR:
            msg = "Communications error";
            break;      
    }
    fprintf(stderr, "%s\n", msg);
    fflush(stderr);
    if (type > NUM_DEALER_ERRS) {
        exit(type - NUM_DEALER_ERRS); // so players exit with the right number
    } else {
        exit(type);
    }
}