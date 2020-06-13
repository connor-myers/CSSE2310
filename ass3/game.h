#ifndef GAME_H
#define GAME_H

#include "path.h"
#include "player.h"

typedef struct {
    int numPlayers; // the number of players in game
    Path path; // the path in this game
    Player* players; // the players in this game
} Game;

#endif