#ifndef TYPES
#define TYPES

#include <stdbool.h>

/**
 * Header file for declaring custom types
 */

typedef enum PlayerType {COMPUTER_ZERO, COMPUTER_ONE, HUMAN} PlayerType;
typedef enum PlayerTurn {PLAYER_O_TURN, PLAYER_X_TURN} PlayerTurn;

typedef struct Game {
    
    PlayerType playerOType;
    PlayerType playerXType;
    PlayerTurn playerTurn;
    bool gameOver;
} Game;

typedef struct Board {

    int width;
    int height;
    char*** values;
} Board;

typedef struct Coordinates {

    int row;
    int column;
} Coordinates;

#endif