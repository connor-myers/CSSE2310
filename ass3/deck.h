#ifndef DECK_H
#define DECK_H

typedef struct {
    char* values; // the cards in the deck
    int size; // the number of cards in the deck
    int count; // the number of cards that have been dealt from the deck.
} Deck;

#endif