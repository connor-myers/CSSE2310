#include <stdio.h>
#include "types.h"
#include "utility.h"

/**
 * Prints the board values to the terminal
 * @param board a pointer to the board struct declared in main.c
 */
void print_board(Board* board) {

    for (int i = 0; i < board->height; i++) {
        for (int j = 0; j < board->width; j++) {
            printf("%s", board->values[i][j]);
        }
        printf("\n");
    }
}

/**
 * Prints the human move prompt, using that player's symbol (i.e. O or X) to
 * the terminal
 * @param playerTurn the player whose turn it is 
 */
void print_human_move_prompt(PlayerTurn playerTurn) {

    char symbol = player_enum_to_symbol(playerTurn);    
    printf("%c:(R C)> ", symbol);
}


/**
 * Prints the message to stdin that indicated where the computer placed
 * their marker
 * @param playerTurn the player whose turn it is
 * @param coords the coordinates where the computer placed their marker
 */
void print_computer_placed_move(PlayerTurn playerTurn, Coordinates coords) {
    
    char symbol = player_enum_to_symbol(playerTurn);
    printf("Player %c placed at %d %d\n", symbol, coords.row, coords.column);
}

