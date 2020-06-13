#ifndef PLAYER_H
#define PLAYER_H

typedef enum {
    A = 0, // the index of A in cards array
    B = 1, // the index of B in cards array
    C = 2, // the index of C in cards array
    D = 3, // the index of D in cards array
    E = 4  // the index of E in cards array
} CardsIndex;

typedef struct {
    int playerId; // the identifying number for the player
    int site; // the site the player is at
    int money; // how much money the player has
    int v1; // the number of v1 type sites this player has visited
    int v2; // the number of v2 type sites this player has visited
    int points; // the number of points this player has
    int* cards; // array with cards the player has
} Player;

#endif