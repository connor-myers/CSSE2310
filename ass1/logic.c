#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "exit.h"
#include "types.h"
#include "utility.h"
#include "logic.h"

/**
 * Checks whether the cell at the specified coordinates is empty or not
 * @param coordinates the coordinates of the cell
 * @param board the main board struct
 * @returns true if the cell is empty (i.e. has a '.' in it), false otherwise
 */
bool check_cell_empty(Coordinates coordinates, Board* board) {
    return ((board->values[coordinates.row][coordinates.column][1]) == '.');
}

/**
 * Determines if the location the player/computer wants to place their marker
 * is valid
 * @param coordinates coordinates struct containing the row and col to check
 * @param board the main game board struct 
 */
bool check_valid_placement(Coordinates coordinates, Board* board) {

    bool valid = true;

    if (coordinates.row >= board->height
            || coordinates.column >= board->width) {
        // we return here so program doesn't try to read invalid memory
        return false; 
    }

    if (coordinates.row < 0 || coordinates.column < 0) {
        return false;
    }

    if (board->values[coordinates.row][coordinates.column][1] != '.') {
        valid = false;
    }

    // need to check push validity
    // Stones can only be played in an edge cell if:
    // 1. There is an empty cell in the direction it would be pushed.
    // 2. There is a stone to be pushed immediately next to the edge cell

    if (coordinates.row == 0 || coordinates.row == board->height - 1) {
        valid = check_vertical_push_valid(coordinates, board);
    }

    if (coordinates.column == 0 || coordinates.column == board->width - 1) {
        valid = check_horizontal_push_valid(coordinates, board);
    }
    
    return valid;
}

/**
 * Checks to see if a marker placed on an vertical edge is valid.
 * I.e., it checks to see if there is
 * 1. a marker existing already to be pushed
 * 2. at least one spot for the one or more markers in the row to be pushed to
 * NOTE: Make sure you only call this function if you *know* the row = 0
 * @param coordinates the coordinates of the location on the board to check
 * @param board the main game board struct
 */
bool check_vertical_push_valid(Coordinates coordinates, Board* board) {
    
    bool valid = false; 

    Coordinates adjacentCell; // coordinates for the adjacent cell
    adjacentCell.column = coordinates.column; // has same column

    if (coordinates.row == 0) {
        adjacentCell.row = 1;
    }

    if (coordinates.row == board->height - 1) {
        adjacentCell.row = board->height - 2;
    }

    // the adjacent cell must be filled, thus the !
    valid = !check_cell_empty(adjacentCell, board);

    valid = valid && (sum_empty_cells_in_col(coordinates.column, board) >= 2);

    return valid;
}

/**
 * Checks to see if a marker placed on a horizontal edge is valid.
 * I.e., it checks to see if there is
 * 1. a marker existing already to be pushed
 * 2. at least one spot for the one or more markers in the col to be pushed to
 * NOTE: Make sure you only call this function if you *know* the col = 0
 * @param coordinates the coordinates of the location on the board to check
 * @param board the main game board struct
 */
bool check_horizontal_push_valid(Coordinates coordinates, Board* board) {

    bool valid = false; 

    Coordinates adjacentCell; // coordinates for the adjacent cell
    adjacentCell.row = coordinates.row; // has the same row

    if (coordinates.column == 0) {
        adjacentCell.column = 1;
    }

    if (coordinates.column == board->width - 1) {
        adjacentCell.column = board->width - 2;
    }

    valid = !check_cell_empty(adjacentCell, board);
    valid = valid && (sum_empty_cells_in_row(coordinates.row, board) >= 2);
    
    return valid;
}

/**
 * After a valid marker is placed, this function checks the entire board to 
 * see if anything needs to be pushed.
 * @param board the main game board struct
 * @param lastPlaced the coordinates of the marker that was just placed
 * (this is included so rows or columns with 1 empty space on the end doesn't
 * just push back and forth every turn)
 */
void push_markers(Board* board, Coordinates lastPlaced) {

    push_rows(board, lastPlaced);
    push_cols(board, lastPlaced);
}

/**
 * Checks all the rows and pushes if anything needs to be pushed
 * @param board the main game board struct
 * @param lastPlaced the coordinates of the marker that was just placed
 */
void push_rows(Board* board, Coordinates lastPlaced) {

    Coordinates coords;
    for (int i = 1; i < board->width - 1; i++) {

        int emptyCellsInCol = sum_empty_cells_in_col(i, board);

        if (emptyCellsInCol == 0) {
            continue;
        }

        if (board->values[0][i][1] != '.') {
            coords.row = 0;
            coords.column = i;

            if (emptyCellsInCol == 1
                    && !coordinates_equal(lastPlaced, coords)) {
                continue;                
            }

            push_vertical(coords, board, true);

            return;           
        } else if (board->values[board->height - 1][i][1] != '.') {
            coords.row = board->height - 1;
            coords.column = i;

            if (emptyCellsInCol == 1
                    && !coordinates_equal(lastPlaced, coords)) {
                continue;                
            }

            push_vertical(coords, board, false);

            return;       
        }
    }
}

/**
 * Checks all the cols and pushes if anything needs to be pushed
 * @param board the main game board struct
 * @param lastPlaced the coordinates of the marker that was just placed
 */
void push_cols(Board* board, Coordinates lastPlaced) {

    Coordinates coords;
    for (int i = 1; i < board->height - 1; i++) {

        int emptyCellsInRow = sum_empty_cells_in_row(i, board);

        if (emptyCellsInRow == 0) {
            continue;
        }

        if (board->values[i][0][1] != '.') {
            coords.row = i;
            coords.column = 0;

            if (emptyCellsInRow == 1
                    && !coordinates_equal(lastPlaced, coords)) {
                continue;                
            }

            push_horizontal(coords, board, true); 

            return;          
        } else if (board->values[i][board->width - 1][1] != '.') {
            coords.row = i;
            coords.column = board->width - 1;

            if (emptyCellsInRow == 1
                    && !coordinates_equal(lastPlaced, coords)) {
                continue;                
            }

            push_horizontal(coords, board, false);

            return;
        }
    }    
}

/**
 * Recursive algorithm which Pushes the marker at coords up/down 
 * if there is an empty spot.
 * If there isn't an empty spot, push up/down is recursively called on that 
 * non-empty spot 
 * @param coords the coordinates of the marker to be pushed
 * @param board the main game board
 * @param down whether the markers are being pushed down or up
 */
void push_vertical(Coordinates coords, Board* board, bool down) {

    Coordinates adjacentCell = get_adjacent_vertical_cell(coords, board, down);

    if (adjacentCell.row == -1) {
        return;
    }

    if (!check_cell_empty(adjacentCell, board)) {
        push_vertical(adjacentCell, board, down);
    } 

    char symbol = get_symbol(coords, board);
    board->values[adjacentCell.row][adjacentCell.column][1] = symbol;
    board->values[coords.row][coords.column][1] = '.';
}

/**
 * Recursive algorithm which Pushes the marker at coords right/left,
 * if there is an empty spot.
 * If there isn't an empty spot, push down is recursively called on that 
 * non-empty spot 
 * @param coords the coordinates of the marker to be pushed
 * @param board the main game board
 * @param down whether the markers are being pushed down or up
 */
void push_horizontal(Coordinates coords, Board* board, bool right) {

    Coordinates adjacentCell =
            get_adjacent_horizontal_cell(coords, board, right);

    if (adjacentCell.column == -1) {
        return;
    }

    if (!check_cell_empty(adjacentCell, board)) {
        push_horizontal(adjacentCell, board, right);
    } 

    char symbol = get_symbol(coords, board);
    board->values[adjacentCell.row][adjacentCell.column][1] = symbol;
    board->values[coords.row][coords.column][1] = '.';
}

/**
 * Looks through the values on the board and determines if the game is over or
 * not
 * @param board the main game board struct
 * @return true if the game is over, false otherwise.
 */
bool check_game_over(Board* board) {

    for (int i = 1; i < board->height - 1; i++) {
        for (int j = 1; j < board->width - 1; j++) {
            if (board->values[i][j][1] == '.') {
                return false; // only need one space
            }
        }
    }

    return true;
}

/**
 * Scans through the board values and calculates the winner
 * @param board the main game board struct
 * @returns string with appropriate winning message
 */
char* calculate_winner(Board* board) {

    int naughtScore = 0;
    int crossScore = 0;

    for (int i = 1; i < board->height - 1; i++) {
        for (int j = 1; j < board->width - 1; j++) {
            if (board->values[i][j][1] == 'O') {
                naughtScore += (board->values[i][j][0] - '0');                
            }
            if (board->values[i][j][1] == 'X') {
                crossScore += (board->values[i][j][0] - '0');                
            }
        }
    }

    if (naughtScore == crossScore) {
        return "O X";
    }

    if (naughtScore > crossScore) {
        return "O";
    } else {
        return "X";
    }
}

/**
* Checks all the edges of the board and sees if player can make a move that
* would lower the score of the other player
* @param board the main game board struct
* @param player the player who is making the move; the one lowering the score
* of the other player
* @returns the coordinates of the *first* edge found that lowers the score of
* the other player. If no such coordinate can be found, returns invalid coords
* where row and column both = -1
*/
Coordinates find_lower_score(Board* board, PlayerTurn player) {

    Coordinates coords;
    coords.row = -1;
    coords.column = -1;
    Coordinates edge;

    PlayerTurn otherPlayer = player ^= 1;

    // check top row
    edge = find_lower_score_top_row(board, otherPlayer);
    if (edge.row != -1 && edge.column != -1) {
        return edge;
    }
    

    // check right column
    edge = find_lower_score_right_col(board, otherPlayer);
    if (edge.row != -1 && edge.column != -1) {
        return edge;
    }

    // check bottom row
    edge = find_lower_score_bottom_row(board, otherPlayer);
    if (edge.row != -1 && edge.column != -1) {
        return edge;
    }

    // check left column
    edge = find_lower_score_left_col(board, otherPlayer);
    if (edge.row != -1 && edge.column != -1) {
        return edge;
    }
    
    return coords;
}

/**
* Checks the top row and see if there is an edge push that would lower
* otherPlayer's score
* @param board the main game board struct
* @param otherPlayer the PlayerTurn of the player whose score is to be lowered
*/
Coordinates find_lower_score_top_row(Board* board, PlayerTurn otherPlayer) {

    // If we can't find a way to lower score, return invalid coords at -1,-1
    Coordinates coords;
    coords.row = -1;
    coords.column = -1;

    for (int j = 1; j < board->width - 1; j++) {
        Coordinates edge;
        edge.row = 0;
        edge.column = j;

        if (!check_vertical_push_valid(edge, board)) {
            continue;
        }
        
        int prePushScore = calculate_score(board, otherPlayer);
        Board boardCopy = copy_board(board);
        push_vertical(edge, &boardCopy, true);
        int postPushScore = calculate_score(&boardCopy, otherPlayer);
        free_board_values(boardCopy.values, boardCopy.height, boardCopy.width);

        if (postPushScore < prePushScore) {
            return edge;
        }
    }

    return coords;
}

/**
* Checks the right col and see if there is an edge push that would lower
* otherPlayer's score
* @param board the main game board struct
* @param otherPlayer the PlayerTurn of the player whose score is to be lowered
*/
Coordinates find_lower_score_right_col(Board* board, PlayerTurn otherPlayer) {

    // If we can't find a way to lower score, return invalid coords at -1,-1
    Coordinates coords;
    coords.row = -1;
    coords.column = -1;

    for (int row = 1; row < board->height - 1; row++) {
        Coordinates edge;
        edge.row = row;
        edge.column = board->width - 1;

        if (!check_horizontal_push_valid(edge, board)) {
            continue;
        }

        // find index where the last push will happen to 
        int prePushScore = calculate_score(board, otherPlayer);
        Board boardCopy = copy_board(board);
        push_horizontal(edge, &boardCopy, false);
        int postPushScore = calculate_score(&boardCopy, otherPlayer);
        free_board_values(boardCopy.values, boardCopy.height, boardCopy.width);

        if (postPushScore < prePushScore) {
            return edge;
        }
    }

    return coords;
}

/**
* Checks the bottom row and see if there is an edge push that would lower
* otherPlayer's score
* @param board the main game board struct
* @param otherPlayer the PlayerTurn of the player whose score is to be lowered
*/
Coordinates find_lower_score_bottom_row(Board* board, PlayerTurn otherPlayer) {

    // If we can't find a way to lower score, return invalid coords at -1,-1
    Coordinates coords;
    coords.row = -1;
    coords.column = -1;

    for (int j = board->width - 2; j > 0; j--) {
        Coordinates edge;
        edge.row = board->height - 1;
        edge.column = j;

        if (!check_vertical_push_valid(edge, board)) {
            continue;
        }

        // find index where the last push will happen to 
        int prePushScore = calculate_score(board, otherPlayer);
        Board boardCopy = copy_board(board);
        push_vertical(edge, &boardCopy, false);
        int postPushScore = calculate_score(&boardCopy, otherPlayer);
        free_board_values(boardCopy.values, boardCopy.height, boardCopy.width);

        if (postPushScore < prePushScore) {
            return edge;
        }
    }

    return coords;
}

/**
* Checks the left col and see if there is an edge push that would lower
* otherPlayer's score
* @param board the main game board struct
* @param otherPlayer the PlayerTurn of the player whose score is to be lowered
*/
Coordinates find_lower_score_left_col(Board* board, PlayerTurn otherPlayer) {

    // If we can't find a way to lower score, return invalid coords at -1,-1
    Coordinates coords;
    coords.row = -1;
    coords.column = -1;

    for (int row = board->height - 2; row > 0; row--) {
        Coordinates edge;
        edge.row = row;
        edge.column = 0;

        if (!check_horizontal_push_valid(edge, board)) {
            continue;
        }

        // find index where the last push will happen to 
        int prePushScore = calculate_score(board, otherPlayer);
        Board boardCopy = copy_board(board);
        push_horizontal(edge, &boardCopy, true);
        int postPushScore = calculate_score(&boardCopy, otherPlayer);
        free_board_values(boardCopy.values, boardCopy.height, boardCopy.width);

        if (postPushScore < prePushScore) {
            return edge;
        }
    }
    
    return coords;
}

/**
* Searches the board and finds the value of the highest cell on the board
* that doesn't already have a marker placed on it; i.e. it's free and a marker
* could be placed there
* @param board the main game board struct
* @returns the value of the highest unmarked cell on the board
*/
int find_highest_free_cell(Board* board) {

    int max = 0;
    for (int row = 1; row < board->height - 1; row++) {
        for (int col = 1; col < board->width - 1; col++) {
            int cellValue = board->values[row][col][0] - '0';
            if (board->values[row][col][1] == '.' && cellValue > max) {
                max = cellValue;
            }
        }
    }

    return max;
}
