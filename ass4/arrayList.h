#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <string.h>
#include <stdlib.h>

/* Definitions */

#define INITIAL_CAPACITY 2      // All lists start out holding 2 members.
#define INITIAL_SIZE 0          // All lists start with 0 members.
#define GROWTH_RATE 2           // How much to grow array by when full.
#define CAPACITY_BUFFER 2       // Always have 2 more capacity than size.
#define INVALID_INDEX -1        // Represents an invalid index in the list.

/* Types */

/*
    summary:    Enum for all the possible ArrayList errors
*/
typedef enum ListError {
    NO_ERROR,                   // No error.
    BAD_ENTRY_SIZE,             // Size of entry is wrong.
    INDEX_OUT_OF_BOUNDS,        // Index is greater than array size.
} ListError;

/*
    summary:    Struct for holding Array List details
*/
typedef struct ArrayList {
    void **entries;             // The entries in the list.
    size_t size;                // How many entries the list has.
    size_t capacity;            // How many entries the list can fit.
    size_t sizeEntry;           // The size of a single entry into list.
} ArrayList;

/* Public Functions */

void arraylist_init(ArrayList *list, size_t sizeMember);
void arraylist_add(ArrayList *list, void *entry, ListError *error);
void *arraylist_get(ArrayList *list, unsigned int index, ListError *error);
void arraylist_free(ArrayList *list);

#endif