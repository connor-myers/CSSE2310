#ifndef ERRS_H
#define ERRS_H

#include <stdio.h>
#include <stdlib.h>

// the number of errors which are for the dealer
// i.e. all errors above NUM_DEALER_ERRS are the player errors
#define NUM_DEALER_ERRS 5

typedef enum {
    GOOD = 0, // program completed with no problems
    BAD_DEALER_ARGS = 1, // dealer was started with an incorrect num of args
    BAD_DECK = 2, // problem with deck file or its contents
    BAD_DEALER_PATH = 3, // problem with path file or its contents
    BAD_PROCESS = 4, // problem with one or more of the player processes
    DEALER_COM_ERROR = 5, // communications error
    BAD_PLAYER_ARGS = 6, // player was started with incorrect number of args
    BAD_PLAYER_COUNT = 7, // player created with an invalid number of players
    BAD_PLAYER_ID = 8, // player id is invalid
    BAD_PLAYER_PATH = 9, // error in path given to player
    EARLY_GAME_OVER = 10, // early game over
    PLAYER_COM_ERROR = 11 // communications error in player program
} Err;

void err_msg(Err type);

#endif