#ifndef MAPPER2310_H
#define MAPPER2310_H

#include <signal.h>
#include <pthread.h>
#include "network.h"
#include "utility.h"
#include "arrayList.h"

/* Definition */

#define GOOD_MAPPER_EXIT 0                  // Exit code when mapper exits well
#define VALID_NUM_COLONS 1                  // The valid num colons in message
#define MIN_GET_PORT_LENGTH 2               // Min chars in get port command
#define VALID_DISPLAY_COMMAND_LENGTH 1      // The num chars in "@" command
#define MAP_DELIM_AS_STRING ":"             // Delimiter for map as a string
#define MAP_DELIM MAP_DELIM_AS_STRING[0]    // Delimiter in mapping

/* Types */

/*
    summary:    Struct which represents a singular entry in the map
*/
typedef struct Mapping {
    char *id;           // Airport id
    int port;           // The port the airport is using
} Mapping;

/*
    summary:    Wrapping struct for the thread function call in mapper2310
*/
typedef struct MapperThreadWrapper {
    ArrayList *map;     // Pointer to the mapper's map
    sem_t *lock;        // Semaphore
    int connFd;         // Connection file desicriptor
} MapperThreadWrapper;

/*
    summary:    Enum that Represents possible command-types a control can send
                mapper
*/
typedef enum CommandType {
    GET_PORT = '?',     // The character that signifies the get port command
    ADD_MAPPING = '!',  // The character that signifies the add mapping command
    PRINT_MAP = '@'     // The character that signifies the print map command
} CommandType;

/* Functions */

void *handle_mapper_connection(void *args);
void handle_input(FILE *in, FILE *out, ArrayList *map, sem_t *guard);
void handle_get_port_command(char *message, FILE *out, ArrayList *map);
void handle_add_mapping_command(char *message, FILE *out, ArrayList *map);
void handle_display_map_command(char *message, FILE *out, ArrayList *map);
bool is_id_unique(char *id, ArrayList *map);
int mapping_comparator(const void *mappingOne, const void *mappingTwo);
void setup_signal_handlers(void);
void handle_signals(int signal);

#endif