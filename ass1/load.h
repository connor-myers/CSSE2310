#include "types.h"

// ### ARGUMENT CHECKING FUNCTIONS ###

void check_num_args(int argc);
void check_player_type_values(int argc, char** argv);
void check_valid_board_file(char* fileName);

// ### RETRIEVING DATA FROM ARGS ###

PlayerType* get_player_types(char** argv);
int* get_board_dimensions(char* fileName);
PlayerTurn get_player_turn(char* fileName);
char*** get_values(char* fileName, int height, int width);

Coordinates find_lower_score(Board* board, PlayerTurn player);