#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "errs.h"
#include "init.h"
#include "communication.h"
#include "utility.h"
#include "logic.h"
#include "constants.h"

typedef struct {
    Game game; // main game struct
    Deck deck; // deck struct
    char* pathString; // the path in a string format
    int** dealerToPlayer; // dealer to player file descriptors
    int** playerToDealer; // player to dealer file descriptors
    FILE** dealerToPlayerStreams; // dealer to player streams
    FILE** playerToDealerStreams; // player to dealer streams
    pid_t* playerPids; // process ids for the players
    bool gameStarted; // true iff all players have sent '^' to stdout
} Dealer;

#endif