/**
 * exit.c defines all the non-zero exits from the program 
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * Exit thrown when user inputs incorrect num of args.
 */
void exit_invalid_num_args() {
    
    fprintf(stderr, "%s", "Usage: push2310 typeO typeX fname\n");
    exit(1);
}

/**
 * Exit thrown when user inputs invalid player types.
 */
void exit_invalid_player_type() {
    
    fprintf(stderr, "%s", "Invalid player type\n");
    exit(2);
}

/**
 * Exit thrown when the program can't read the file supplied in arguments.
 */
void exit_load_file_error() {
    
    fprintf(stderr, "%s", "No file to load from\n");
    exit(3);
}

/**
 * Exit thrown when the contents of the user-supplied file is invalid.
 */
void exit_invalid_file() {
    
    fprintf(stderr, "%s", "Invalid file contents\n");
    exit(4);
}

/**
 * Exit thrown when user inputs EOF into stdin when input is requested.
 */
void exit_end_of_file() {
    
    fprintf(stderr, "%s", "End of file\n");
    exit(5);
}

/**
 * Exit thrown when the game being loaded has no empty cells in the interior.
 */
void exit_no_empty_interior_cells() {
    
    fprintf(stderr, "%s", "Full board in load\n");
    exit(6);
}