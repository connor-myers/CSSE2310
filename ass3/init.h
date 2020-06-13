#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "errs.h"
#include "utility.h"
#include "deck.h"
#include "constants.h"

Game init_game(FILE* pathStream, int numPlayers, bool dealer);
Player init_player(int id);
Deck init_deck(FILE* deckStream);
char* init_path_string(FILE* stream);

#endif