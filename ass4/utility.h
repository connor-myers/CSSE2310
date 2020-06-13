#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <semaphore.h> 
#include <signal.h>
#include "exit.h"

/* Definitions */

#define INITIAL_BUFFER_CAPACITY 80  // initial size of message
#define INITIAL_LINE_LENGTH 0       // Line has 0 chars to start with 
#define EMPTY_LINE 0                // An empty line has 0 chars in it 
#define BUFFER_GROWTH_FACTOR 2      // How much to grow buffer by 
#define BUFFER_TOLERANCE 5          // Amount of space before growing buffer
#define READ "r"                    // We want to read on file descriptor
#define WRITE "w"                   // We want to write on file descriptor

/* Types */

/*
    summary:        (StringConversionError) Enum that describes the possible
                    errors when converting string to int.
*/
typedef enum StringConErr {
    FINE,           // No problems converting 
    NULL_STRING,    // String inputted pointed to NULL
    NOT_NUMBER,     // Tried to convert non numeric char/s to int
    WHITE_SPACE,    // String has extra white-space that it shouldn't have 
    LEADING_ZERO    // String has a leading 0; I consider this invalid 
} StringConErr;

/* Functions */

// string utility
char *get_line(FILE *stream);
int string_to_int(char *string, StringConErr *error);
char *int_to_string(int number);
int num_digits(int number);
bool has_invalid_chars(char *string);
int num_char_in_string(char *string, char c);

// semaphore utility
void init_lock(sem_t *lock);
void take_lock(sem_t *lock);
void release_lock(sem_t *lock);

// signal utility
void setup_signal_handlers(void);
void handle_signals(int signal);

#endif