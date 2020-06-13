#include <stdbool.h>
#include "types.h"

void check_allocated_memory(void* ptr);
char player_enum_to_symbol(PlayerTurn PlayerTurn);
PlayerTurn player_symbol_to_enum(char symbol);
bool check_valid_player_type(char* type);
PlayerType string_to_player_type(char* value);
char*** allocate_board_memory(int height, int width);
bool coordinates_equal(Coordinates coords1, Coordinates coords2);
char get_symbol(Coordinates coords, Board* board);
Coordinates get_adjacent_vertical_cell
        (Coordinates coords, Board* board, bool down);
Coordinates get_adjacent_horizontal_cell
        (Coordinates coords, Board* board, bool right);     
void free_board_values(char*** values, int height, int width);
int sum_empty_cells_in_row(int col, Board* board);
int sum_empty_cells_in_col(int col, Board* board);
void place_marker(PlayerTurn playerTurn, Coordinates coords, Board* board);
bool check_full_load(char*** values, int height, int width);
Board copy_board(Board* board);
int calculate_score(Board* board, PlayerTurn playerTurn);