#ifndef PLAYERS_H
#define PLAYERS_H

#include <stdio.h>
#include <stdlib.h>
#include "logic.h"
#include "init.h"
#include "errs.h"
#include "utility.h"
#include "communication.h"
#include "constants.h"

typedef struct {
    int numPlayers; // the number of players in the game
    int id; // the id of the player this program is playing as
} ProgramInfo;

void run(int argc, char** argv, int (*moveLogic)(Game*, Player*));
ProgramInfo check_args(int argc, char** argv);
void ready(void);
void play(int (*moveLogic)(Game*, Player*), Game* game, int playerId);
void update(Game* game, Message message);
Message decode_dealer_message(char* message);
MessageType decode_dealer_message_type(char* message);
int* decode_hap_params(char* message);
void finish(Game game);

#endif