/**
 * This file handles how both computer types will interact with the game board 
 */

#include <stdio.h>
#include "types.h"
#include "graphics.h"
#include "logic.h"

/**
 * Works out where computer zero would want to place
 * @param board the main game board struct
 * @param player the player whose turn it is
 */
Coordinates get_computer_zero_input(Board* board, PlayerTurn player) {

    Coordinates coords;
    coords.row = -1;
    coords.column = -1;
    
    if (player == PLAYER_O_TURN) {

        for (int i = 1; i < board->height - 1; i++) {
            for (int j = 1; j < board->width - 1; j++) {
                if (board->values[i][j][1] == '.') {
                    coords.row = i;
                    coords.column = j;
                    return coords;
                }                
            }
        }
    } else if (player == PLAYER_X_TURN) {
        
        for (int i = board->height - 2; i > 0; i--) {
            for (int j = board->width - 2; j > 0; j--) {
                if (board->values[i][j][1] == '.') {
                    coords.row = i;
                    coords.column = j;
                    return coords;
                }                
            }
        }
    }

    return coords;
}

/**
 * Works out where computer one would want to place
 * @param board the main game board struct
 * @param player the player whose turn it is
 */
Coordinates get_computer_one_input(Board* board, PlayerTurn player) {

    Coordinates coords;
    coords.row = -1;
    coords.column = -1;
    
    // check edges and see if point lowers score... 
    coords = find_lower_score(board, player);

    if (coords.row == -1 || coords.column == -1) {
        // nothing was found, try find highest value
        
        int max = find_highest_free_cell(board);
        for (int row = 1; row < board->height - 1; row++) {
            for (int col = 1; col < board->width - 1; col++) {
                int value = board->values[row][col][0] - '0';
                if (value == max && (board->values[row][col][1] == '.')) {
                    coords.row = row;
                    coords.column = col;
                    return coords;
                }
            }
        }
    }
    
    return coords;
}



