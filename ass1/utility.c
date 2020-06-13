#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "exit.h"
#include "types.h"
#include "utility.h"

/**
 * Checks to see if dynamically allocated memory is allocated properly.
 * Exits if memory is not allocated properly as this would cause undefined
 * behaviour otherwise
 * 
 * @param ptr a pointer to the dynamically allocated memory.
 */
void check_allocated_memory(void* ptr) {

    // malloc returns NULL if allocation of memory was not successfull
    // Since NULL = 0, !ptr checks if ptr is NULL
    if (!ptr) {
        fprintf(stderr, "%s", "ERROR ALLOCATING MEMORY\n");
        exit(MEMORY_FAILURE_EXIT);
    }
}

/**
 * Converts the PlayerTurn enum to the associated symbol
 * I.e. if it's playerTurn = PLAYER_X_TURN, then it will return 'X'
 * @param playerTurn the player to get the symbol for 
 */
char player_enum_to_symbol(PlayerTurn playerTurn) {

    char symbol;
    
    switch (playerTurn) {

        case PLAYER_O_TURN:
            symbol = 'O';
            break;

        case PLAYER_X_TURN:
            symbol = 'X';
            break;   
    }

    return symbol;
}

/**
 * Converts the player symbol i.e. O or X
 * to the equivalent PlayerTurn enum value
 */
PlayerTurn player_symbol_to_enum(char symbol) {

    PlayerTurn playerTurn;

    switch (symbol) {
        
        case 'O':
            playerTurn = PLAYER_O_TURN;
            break;
        case 'X':
            playerTurn = PLAYER_X_TURN;
            break;
        default:
            // a problem has occured if this is set        
            playerTurn = -1;
    }

    return playerTurn;
}

/**
 * Frees the dynamically allocated memory for the board
 * @param the 3D array which contains for board values
 */
void free_board_values(char*** values, int height, int width) {

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            free(values[i][j]);
        }
        free(values[i]);
    }
        
    free(values);
}

/**
 * Checks if the user supplied player type is valid
 * @param type the user supplied player type
 * @returns true iff the type is valid, i.e. 0, 1 or H.
 */
bool check_valid_player_type(char* type) {

    return (strcmp(type, "0") == 0 || strcmp(type, "1") == 0
            || strcmp(type, "H") == 0);
}

/**
 * Converts the char* value of the user supplied player type to
 * equivalent enum PlayerType value 
 * @param value user supplied player type
 * @returns equivalent enum value for user supplied player type
 */
PlayerType string_to_player_type(char* value) {

    // NOTE: We have already checked if player type inputted were valid
    
    if (strcmp(value, "0") == 0 || strcmp(value, "1") == 0) {
        return value[0] - '0';
    } else {
        // since it must be valid, and it isn't 0 or 1, it must be H
        return HUMAN;
    }
}

/**
 * Dynamically allocates the memory for the board values
 * @param height the height of the board
 * @param width the width of the board
 * @returns dynamically allocated 3D array
 */
char*** allocate_board_memory(int height, int width) {

    char*** values = malloc(sizeof(char**) * height);
    check_allocated_memory(values);

    for (int i = 0; i < height; i++) {
        values[i] = malloc(sizeof(char*) * width);
        check_allocated_memory(values[i]);
        for (int j = 0; j < width; j++) {
            // 3 since 2 characters + null terminator
            values[i][j] = malloc(sizeof(char) * 3);
            check_allocated_memory(values[i][j]);
        }
    }
    
    return values;
}

/**
 * Checks if two coordinates are equal
 * @param coords1 the first pair of coordinates
 * @param coords2 the second pair of coordinates
 * @returns true if both row and column are equal
 */
bool coordinates_equal(Coordinates coords1, Coordinates coords2) {
    
    return ((coords1.row == coords2.row)
            && (coords1.column == coords2.column));
}

/**
 * Gets the non-numerical symbol at certain coords
 * i.e. O or X or .
 * @param coords the coordinates you want to get the marker for
 * @param board the main game board
 * @returns the char symbol at that location on the board
 */
char get_symbol(Coordinates coords, Board* board) {

    return (board->values[coords.row][coords.column][1]);
}

/**
 * Gets the vertically adjacent cell from the cell in coords.
 * @param coords the cell you want to find the adjacent cell to
 * @param board the main game board struct
 * NOTE: in this context adjacent means the cell that the cell in coords
 * would be pushed towards.
 */ 
Coordinates get_adjacent_vertical_cell
        (Coordinates coords, Board* board, bool down) {

    Coordinates adjacentCell;
    adjacentCell.column = coords.column;

    if (down) {
        adjacentCell.row = ++coords.row;
    } else {
        adjacentCell.row = --coords.row;
    }

    if (adjacentCell.row >= board->height || adjacentCell.row < 0) {
        adjacentCell.row = -1;
    }

    return adjacentCell;
}

/**
 * Gets the horizontally adjacent cell from the cell in coords.
 * @param coords the cell you want to find the adjacent cell to
 * @param board the main game board struct
 * NOTE: in this context adjacent means the cell that the cell in coords
 * would be pushed towards. 
 */ 
Coordinates get_adjacent_horizontal_cell
        (Coordinates coords, Board* board, bool right) {

    Coordinates adjacentCell;
    adjacentCell.row = coords.row;

    if (right) {
        adjacentCell.column = ++coords.column;
    } else {
        adjacentCell.column = --coords.column;
    }

    if (adjacentCell.column >= board->width || adjacentCell.column < 0) {
        adjacentCell.column = -1;
    }

    return adjacentCell;
}

/**
 * Counts how many empty cells (i.e. cells that equal '.') in a row
 * @param row the row to check
 * @param board the main game board struct 
 * @returns the number of empty cells in the row
 */
int sum_empty_cells_in_row(int row, Board* board) {

    int numEmpty = 0;

    for (int i = 0; i < board->width; i++) {
        if (board->values[row][i][1] == '.') {
            numEmpty++;
        }
    }

    return numEmpty;
}

/**
 * Counts how many empty cells (i.e. cells that equal '.') in a column
 * @param col the column to check
 * @param board the main game board struct 
 * @returns the number of empty cells in the row
 */
int sum_empty_cells_in_col(int col, Board* board) {

    int numEmpty = 0;

    for (int i = 0; i < board->height; i++) {
        if (board->values[i][col][1] == '.') {
            numEmpty++;
        }
    }

    return numEmpty;
}


/**
 * Places the appropriate marker at the location in board specified by coords
 * @param playerTurn which player's turn it is
 * @param coords the coordinates struct for where the player wants to mark
 * @param the main game board struct 
 * NOTE: This function does *not* check for a valid placement. The coordinates
 * should be checked and confirmed to be valid before being passed into this
 * function
 */
void place_marker(PlayerTurn playerTurn, Coordinates coords, Board* board) {
    
    char marker = player_enum_to_symbol(playerTurn);
    board->values[coords.row][coords.column][1] = marker;
}

/**
 * Looks through the values on the board and determines if the game is over or
 * not
 * @param values the board values
 * @param height the height of the board
 * @param width the width of the board
 * @return true if the game is over, false otherwise.
 */
bool check_full_load(char*** values, int height, int width) {

    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            if (values[i][j][1] == '.') {
                return false; // only need at least one space
            }
        }
    }

    return true;
}

/**
 * Creates a copy of the main game board struct's values
 */
Board copy_board(Board* board) {

    Board copiedBoard;

    char*** values = allocate_board_memory(board->height, board->width);

    for (int i = 0; i < board->height; i++) {
        for (int j = 0; j < board->width; j++) {
            strcpy(values[i][j], board->values[i][j]);
        }
    }

    copiedBoard.values = values;
    copiedBoard.height = board->height;
    copiedBoard.width = board->width;
    
    return copiedBoard;
}

/**
 * Returns the current score of a player
 * @param board the main game board struct
 * @param playerTurn the player whose turn it is
 */
int calculate_score(Board* board, PlayerTurn playerTurn) {

    int totalScore = 0;
    char symbol = player_enum_to_symbol(playerTurn);

    for (int i = 1; i < board->height - 1; i++) {
        for (int j = 1; j < board->width - 1; j++) {
            if (board->values[i][j][1] == symbol) {
                totalScore += (board->values[i][j][0] - '0');
            }
        }
    }

    return totalScore;
}
