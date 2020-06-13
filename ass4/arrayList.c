/*
 *  My garbage implementation of array list for CSSE2310
 *  Missing some functions that array list normally has, but they aren't
 *  required for this assignment.
 */

#include "arrayList.h"

/* Implementation */

/*
    summary:            initialises a new list

    list:               Pointer to an empty ArrayList struct
    
    sizeMember:         The size of an individual struct member of the list
*/
void arraylist_init(ArrayList *list, size_t sizeMember)
{
    list->capacity = INITIAL_CAPACITY;    
    list->size = INITIAL_SIZE;
    list->sizeEntry = sizeMember;
    list->entries = calloc(list->capacity, sizeMember);
}

/*
    summary:            Adds a new entry to the array list

    list:               Pointer to the list

    entry:              An entry in the array list

    error:              Pointer to ListError. Set > 0 if an error occured

    Note:               Won't add if entry isn't the same size
                        as size of members  initially specified
                        in init_list function by sizeMember    
*/
void arraylist_add(ArrayList *list, void *entry, ListError *error)
{
    // Don't add if trying to add a different sized pointer
    if (sizeof(entry) != list->sizeEntry) {
        *error = BAD_ENTRY_SIZE;
        return;
    }

    // If our buffer is full, we need to expand it    
    if (list->size + CAPACITY_BUFFER >= list->capacity) {
        list->capacity *= GROWTH_RATE;
        list->entries = realloc(list->entries,
                list->sizeEntry * list->capacity);
    }
    
    // safely add entry now that buffer has room
    list->entries[list->size] = entry;
    list->size++;
}

/*
    summary:            Gets the entry at the specified index and returns it

    list:               Pointer to the list

    index:              The position in the list to retrieve entry from

    error:              Set >0 if an error occured getting entry
*/
void *arraylist_get(ArrayList *list, unsigned int index, ListError *error)
{
    // Don't try to retrieve an index that hasn't been filled
    if (index >= list->size) {
        *error = INDEX_OUT_OF_BOUNDS;
        return NULL;
    }

    return list->entries[index];
}

/*
    summary:            Releases the memory dynamically allocated by list

    list:               Pointer to the array list

    Note:               If individual entries are dynamically allocated,
                        they should be cleaned up before this functions is 
                        called.      
*/
void arraylist_free(ArrayList *list)
{
    free(list->entries);
    list = NULL;
}