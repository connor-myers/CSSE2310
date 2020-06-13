#include "utility.h"

/*
    summary:            Gets a line (terminated by '\n', '\r' or EOF) 
                        from stream

    stream:             Stream to retrieve line from

    Returns:            Null-terminated string from stream
                        If no info is sent on stream, this function returns
                        NULL

    Note:               Check this doesn't return NULL before you try and
                        free() the result. free(NULL) will surely make your
                        computer explode.
*/
char *get_line(FILE *stream)
{
    size_t capacity = INITIAL_BUFFER_CAPACITY;
    size_t size = INITIAL_LINE_LENGTH;
    char *buffer = calloc(capacity, sizeof(char));
    
    int c;
    while (c = fgetc(stream), c != EOF) {
        // ensure we have space in the buffer
        if (size + BUFFER_TOLERANCE >= capacity) {
            capacity *= BUFFER_GROWTH_FACTOR;
            buffer = realloc(buffer, capacity * sizeof(char));
        }

        // Line is terminated with '\n' or '\r', so replace these with '\0'
        if (c != '\n' && c != '\r') {
            buffer[size++] = c;
        } else {
            buffer[size++] = '\0';
            break; // we're done reading
        }
    }

    // Checking if there was any content before EOF
    if (c == EOF) {
        if (size == EMPTY_LINE) {
            free(buffer);
            buffer = NULL;
        } else {
            buffer[size++] = '\0';
        }    
    }    

    return buffer;
}

/*
    summary:            Safely converts a string to an integer

    string:             The string to be converted to int

    error:              Set if there was an error converting.
                        Should be set to GOOD before passing it in
                        You're asking for problems if you don't.

    Returns:            If error is not set, it returns int version
                        of string If error is set, it's anyone's
                        guess what value it returns                

    Note:               Don't trust the return value is error is set as this
                        function does not terminate early if there is an error.
*/
int string_to_int(char *string, StringConErr *error)
{
    int number;

    if (string != NULL) { // NULL pointer makes strtol sad
        char *token;
        number = (int) strtol(string, &token, 10); // 10 for base-10
        if ((strlen(token) != 0) || (strlen(string) == 0)) {
            *error = NOT_NUMBER;
        }

        // Ensuring any additional white-space invalidates conversion
        for (int i = 0; i < strlen(string); i++) {
            if (string[i] == ' ' || string[i] == '\t') {
                *error = WHITE_SPACE; // having any white-space is bad
            }
        }

        // ensuring there is no leading 0
        if (strlen(string) > 1 && string[0] == '0') {
            *error = LEADING_ZERO; // string had white-space
        }
    } else {
        *error = NULL_STRING;
    }

    return number;
}

/*
    summary:            Converts an integer into a string

    number:             The integer to be converted to a string

    returns:            Integer as a null-terminated,
                        dynamically allocated char*

*/
char *int_to_string(int number)
{
    /* the + 1 in malloc is for the null terminator */
    char *string = malloc(sizeof(char) * num_digits(number) + 1); 
    sprintf(string, "%d", number);

    return string;
}

/*
    summary:            Gets the number of digits (base-10) in an integer

    number:             The integer to find the number of digits for

    returns:            The number of digits (base-10) in a number
*/
int num_digits(int number)
{
    int digits = 0; // a number has zero digits to start off with
    do {
        digits++;
        number /= 10; // 10 for base-10
    } while (number != 0);
    
    return digits;
}

/*
    summary:            Checks to see if the inputted string has any 
                        invalid characters
                        Returns true iff string has invalid characters

    string:             Any null-terminated char*

    returns:            True iff string has invalid characters                
*/
bool has_invalid_chars(char *string)
{
    char c;
    for (int i = 0; i < strlen(string); i++) {
        c = string[i];
        if (c == '\n' || c == '\r' || c == ':') {
            return true;
        }
    }

    return false;
}

/*
    summary:            Counts the number of occurances of a specific
                        char in a string

    string:             The string to be checked for a certain char

    c:                  The character to be searched for

    Returns             The number of times that the character 'c' occurs in
                        the string.
*/
int num_char_in_string(char *string, char c)
{
    int count = 0;
    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == c) {
            count++;
        }
    }
    
    return count;
}

/*
    summary:            Initialises semaphore lock

    lock:               Pointer to lock to be initialised
*/
void init_lock(sem_t *lock)
{
    sem_init(lock, 0, 1);
}

/*
    summary:            Takes the lock

    lock:               Pointer to lock to be taken
*/
void take_lock(sem_t *lock)
{
    sem_wait(lock);
}

/*
    summary:            Releases lock

    lock:               Pointer to the lock to be released
*/
void release_lock(sem_t *lock)
{
    sem_post(lock);
}

/*
    summary:    Sets up signal handlers
                (For this assignment, we only care about SIGINT)

    Note:       Code is taken from sig.c from lecture 5.2
*/
void setup_signal_handlers(void)
{
    struct sigaction sa;
    sa.sa_handler = handle_signals;   
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, 0);
}

/*
    summary:    Handles signals. I.e. when we receieve any signal redirect
                the signal to appropriate handler.
                If we receive SIGINT, (The only signal that should be here)
                this function terminates the calling program with
                "GOOD" exit code

    signal:     The signal that has been thrown            
*/
void handle_signals(int signal)
{
    if (signal == SIGINT) {
        exit(GOOD); // we want to exit normally when user inputs ^C 
    }
}