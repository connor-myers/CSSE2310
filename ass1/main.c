#include <stdio.h>
#include <stdlib.h>
#include "load.h"
#include "types.h"
#include "utility.h"
#include "graphics.h"
#include "input.h"
#include "logic.h"
#include "computer.h"

/**
 * Checks the program arguments
 * @param argc the number of arguments
 * @param argv the arguments
 */
void check_arguments(int argc, char** argv) {

    check_num_args(argc);
    check_player_type_values(argc, argv);
    check_valid_board_file(argv[3]);
}

/**
 * Loads the player types from program arguments
 * @param game pointer to main game object declared in main
 * @param argv program arguments
 * (note, don't need argc as we have already verified there are 4 arguments)
 */
void load_player_types(Game* game, char** argv) {

    PlayerType* playerTypes = get_player_types(argv);
    game->playerOType = playerTypes[0];
    game->playerXType = playerTypes[1];
    free(playerTypes); // playerTypes was dynamically allocated, must be freed
}

/**
 * Loads the board dimensions from savefile
 * @param board pointer to main board object declared in main
 * @param saveFileName name of the saveFile for this game
 */
void load_board_dimensions(Board* board, char* saveFileName) {
    
    int* boardDimensions = get_board_dimensions(saveFileName);
    board->height = boardDimensions[0];
    board->width = boardDimensions[1];
    free(boardDimensions); // boardDimensions was dynamically allocated
}

/**
 * Handles move input for any type of player
 */
Coordinates handle_move(Game* game, Board* board) {

    Coordinates coordinates;

    if (game->playerTurn == PLAYER_O_TURN) {
        if (game->playerOType == HUMAN) {
            coordinates = get_player_input(game->playerTurn, board);
        } else {
            if (game->playerOType == COMPUTER_ZERO) {
                coordinates = get_computer_zero_input(board, PLAYER_O_TURN);
            } else if (game->playerOType == COMPUTER_ONE) {
                coordinates = get_computer_one_input(board, PLAYER_O_TURN);
            }
            print_computer_placed_move(game->playerTurn, coordinates);
        }
    }

    if (game->playerTurn == PLAYER_X_TURN) {
        if (game->playerXType == HUMAN) {
            coordinates = get_player_input(game->playerTurn, board);
        } else {
            if (game->playerXType == COMPUTER_ZERO) {
                coordinates = get_computer_zero_input(board, PLAYER_X_TURN);
            } else if (game->playerXType == COMPUTER_ONE) {
                coordinates = get_computer_one_input(board, PLAYER_X_TURN);
            }
            print_computer_placed_move(game->playerTurn, coordinates);
        }
    }

    return coordinates;
}

int main(int argc, char** argv) {

    // initialising variables
    Board board; // struct with information about the game board
    Game game; // struct with information about the current game
    char* saveFileName; // the name of the savefile 

    // declaring values
    game.gameOver = false;

    // checking arguments
    check_arguments(argc, argv);

    // if this line is reached, args are valid.
    saveFileName = argv[3];

    // loading values
    load_player_types(&game, argv);
    load_board_dimensions(&board, saveFileName);
    game.playerTurn = get_player_turn(saveFileName);
    board.values = get_values(saveFileName, board.height, board.width);

    // main game loop
    Coordinates coordinates;
    while (!game.gameOver) {

        print_board(&board);

        // coordinates = get_player_input(game.playerTurn, &board);
        coordinates = handle_move(&game, &board);
        place_marker(game.playerTurn, coordinates, &board);
        push_markers(&board, coordinates); 

        game.playerTurn ^= 1;

        game.gameOver = check_game_over(&board);
    }

    print_board(&board);

    // calculate winner
    char* winner = calculate_winner(&board);
    printf("Winners: %s\n", winner);

    // board values were allocated dynamically, must free
    free_board_values(board.values, board.height, board.width);
    
    return 0;
}
