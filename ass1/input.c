#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "graphics.h"
#include "utility.h"
#include "types.h"
#include "logic.h"
#include "exit.h"

#define MAX_LINE 80 // this is the max input length according to spec

void save_game(char* saveFileName, Board* board, PlayerTurn playerTurn);

/**
 * Prompts the player for input until a valid row and column is entered and
 * returns coordinates
 * @param playerTurn which player's turn it is
 * @param board pointer to main game board struct
 * @returns Coordinates struct containing row and column specified
 */
Coordinates get_player_input(PlayerTurn playerTurn, Board* board) {

    Coordinates coordinates;
    bool valid = false;

    char* buffer = malloc(sizeof(char) * MAX_LINE);
    do {
        print_human_move_prompt(playerTurn);
        
        if (fgets(buffer, MAX_LINE, stdin) == NULL) {
            exit_end_of_file();
        }

        // new line character is counted, so if only one character is entered
        if (strlen(buffer) == 2) {
            continue; // one character input is always invalid
        }

        // player wants to save
        if (buffer[0] == 's') {
            strtok(buffer, "\n"); // remove trailing '\n'
            save_game(buffer + 1, board, playerTurn); // + 1 removes s
            continue;
        }

        // player making move input 
        coordinates.row = atoi(strtok(buffer, " "));
        coordinates.column = atoi(strtok(NULL, " "));
        valid = check_valid_placement(coordinates, board);
    } while (!valid);

    free(buffer);

    return coordinates;
}

/**
 * Saves the values of board into a file called saveFileName
 * @param saveFileName what the saveFile should be called
 * @param board the board to be saved
 */
void save_game(char* saveFileName, Board* board, PlayerTurn playerTurn) {

    FILE* file = fopen(saveFileName, "w");

    if (file == NULL) {
        fprintf(stderr, "%s", "Save failed\n");
        return;
    }

    char* buffer = malloc(sizeof(char) * MAX_LINE);

    // adding rows and cols to top of file
    sprintf(buffer, "%d %d\n", board->height, board->width);
    fputs(buffer, file);

    // adding player turn to file
    sprintf(buffer, "%c\n", player_enum_to_symbol(playerTurn));
    fputs(buffer, file);

    for (int i = 0; i < board->height; i++) {
        for (int j = 0; j < board->width; j++) {
            fputs(board->values[i][j], file);
        }
        fputs("\n", file);
    }

    fclose(file);
    free(buffer);
}