#ifndef UTILITY_H
#define UTILITY_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "game.h"
#include "message.h"
#include "constants.h"

int string_to_int(char* string, int* error);
char* int_to_string(int number);
int num_digits(int number);
char* get_line(FILE* stream, bool* eof);
int calculate_points_from_cards(Player* player);
int calculate_points_from_sites(Player* player);
void free_game(Game game);
bool is_valid_card(char card);
bool is_valid_hap(Message message, Game game);
void print_player_info(FILE* stream, Player* player);
void print_path(Game game, FILE* stream);
void print_scores(Game* game, FILE* stream);


#endif