/**
 * This file handles loading data from the input arguments and the savefile
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "exit.h"
#include "types.h"
#include "utility.h"

#define MIN_DIMENSION 3
#define MAX_SAVEFILE_LINE 80 // not sure this is correct,
//there is probably a max height or width but documentation doesn't specify

// ### ARGUMENT CHECKING FUNCTIONS ###

/**
 * Checks the number of arguments supplied by user.
 * Exits program with appropriate exit code and message to stderr if invalid
 * number of arguments supplied. Otherwise, it does nothing.
 * 
 * @param argc the number of arguments supplied by the user
 */
void check_num_args(int argc) {

    if (argc != 4) {
        exit_invalid_num_args();
    }
}

/**
 * Checks for any invalid arg values. I.e. player type other than 0, 1 or H.
 * Does not check for invalid number of args, as this function should be called
 * after check_num_args.
 * 
 * @param argc the number of arguments
 * @param argv the arguments
 */
void check_player_type_values(int argc, char** argv) {
    
    char* playerTypeOne = argv[1];
    char* playerTypeTwo = argv[2];  

    if (!check_valid_player_type(playerTypeOne)
            || !check_valid_player_type(playerTypeTwo)) {
        exit_invalid_player_type();
    }
}

/**
 * Checks if the filename supplied by the user in the program arguments exists
 * Exits program with appropriat exit function if file does not exist
 * @param fileName the name of the file
 */
void check_valid_board_file(char* fileName) {

    FILE* file;
    if ((file = fopen(fileName, "r")) != NULL) {
        // if we don't close, it might cause errors later when we open it again
        fclose(file); 
    } else {
        exit_load_file_error();
    }
}

// ### RETRIEVING DATA FROM ARGS ###

/**
 * Gets the player types from the arguments supplied by user
 * @param argv the program arguments
 * @returns An array containing the values for playerTypes
 */
PlayerType* get_player_types(char** argv) {
    
    PlayerType* playerTypes = malloc(sizeof(PlayerType) * 2);

    // first, we need to check if memory was allocated successfully
    check_allocated_memory((void*) playerTypes);

    playerTypes[0] = string_to_player_type(argv[1]);
    playerTypes[1] = string_to_player_type(argv[2]);

    return playerTypes;    
}

/**
* Opens file called fileName and retrieves dimensions of board
* @param fileName the name of the savefile
* @returns int array containing height of board at index 0
* and width of board at index 1
*/
int* get_board_dimensions(char* fileName) {

    int* dimensions = malloc(sizeof(int) * 2);

    FILE* file;
    if ((file = fopen(fileName, "r")) == NULL) {
        exit_load_file_error();
    }

    char buffer[MAX_SAVEFILE_LINE];
    fgets(buffer, MAX_SAVEFILE_LINE, file);

    char* height = strtok(buffer, " ");
    char* width = strtok(NULL, " ");

    dimensions[0] = atoi(height);
    dimensions[1] = atoi(width);

    // If atoi fails/can't make a conversion, it returns 0.
    // 0 is an invalid dimensions anyway.
    // So, we don't need to specifically check if atoi failed.
    if (dimensions[0] < MIN_DIMENSION || dimensions[1] < MIN_DIMENSION) {
        exit_invalid_file();
    }

    fclose(file);

    return dimensions;    
}

/**
 * Retrieves which player's turn it is from the savefile
 * @param fileName the name of the save file
 * @returns which player's turn is next
 */
PlayerTurn get_player_turn(char* fileName) {

    PlayerTurn playerTurn;

    FILE* file;
    if ((file = fopen(fileName, "r")) == NULL) {
        exit_load_file_error();
    }

    char buffer[MAX_SAVEFILE_LINE];
    fgets(buffer, MAX_SAVEFILE_LINE, file); // this skips the first line
    fgets(buffer, MAX_SAVEFILE_LINE, file);

    // checking for > 2 and not > 1 since all fgets ends with '\n'
    if (strlen(buffer) > 2) {
        exit_invalid_file();
    }

    playerTurn = player_symbol_to_enum(buffer[0]);

    if (playerTurn == -1) {
        exit_invalid_file();
    }

    fclose(file);

    return playerTurn;
}

/**
 * Gets the values of the board from the savefile
 * @param fileName the name of the savefile
 * @returns a multidimensional array containing the values of the board
 */
char*** get_values(char* fileName, int height, int width) {

    FILE* file;
    if ((file = fopen(fileName, "r")) == NULL) {
        exit_load_file_error();
    }

    char buffer[MAX_SAVEFILE_LINE];

    // allocating memory for multidimensional array
    char*** values = allocate_board_memory(height, width);

    // skip first 2 lines of savefile
    for (int i = 0; i < 2; i++) {
        fgets(buffer, MAX_SAVEFILE_LINE, file); 
    }

    // read through each line of board values in savefile
    for (int i = 0; i < height; i++) {

        fgets(buffer, MAX_SAVEFILE_LINE, file);
        strtok(buffer, "\n"); // removes trailing new line character
    
        for (int j = 0; j < width * 2; j += 2) { 

            if ((i == 0 || i == height) && j >= width * 2 - 2) {
                //continue;
                // i don't think is needed, leaving in just in case
            }

            values[i][j / 2][0] = buffer[j];
            values[i][j / 2][1] = buffer[j + 1];   
            values[i][j / 2][2] = '\0';                     
        }
    }

    if (check_full_load(values, height, width)) {
        exit_no_empty_interior_cells();
    }

    fclose(file);
    return values;
}
