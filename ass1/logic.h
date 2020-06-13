#include <stdbool.h>
#include "types.h"

bool check_cell_empty(Coordinates coordinates, Board* board);
bool check_valid_placement(Coordinates coordinates, Board* board);
bool check_horizontal_push_valid(Coordinates coordinates, Board* board);
bool check_vertical_push_valid(Coordinates coordinates, Board* board);

void push_markers(Board* board, Coordinates lastPlaced);
void push_rows(Board* board, Coordinates lastPlaced);
void push_cols(Board* board, Coordinates lastPlaced);
void push_vertical(Coordinates coords, Board* board, bool down);
void push_horizontal(Coordinates coords, Board* board, bool right);

bool check_game_over(Board* board);
char* calculate_winner(Board* board);

Coordinates find_lower_score(Board* board, PlayerTurn player);
int find_highest_free_cell(Board* board);

Coordinates find_lower_score_top_row(Board* board, PlayerTurn otherPlayer);
Coordinates find_lower_score_right_col(Board* board, PlayerTurn otherPlayer);
Coordinates find_lower_score_bottom_row(Board* board, PlayerTurn otherPlayer);
Coordinates find_lower_score_left_col(Board* board, PlayerTurn otherPlayer);