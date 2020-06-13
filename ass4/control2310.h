#ifndef CONTROL2310_H
#define CONTROL2310_H

#include <pthread.h>
#include "exit.h"
#include "utility.h"
#include "network.h"
#include "arrayList.h"

/* Definitions */

#define MIN_CONTROL_ARGS 3          // Min num of args to control to be valid
#define MAX_CONTROL_ARGS 4          // Max num of args to control to be valid
#define NO_MAPPER -1                // Indicates no mapper was specified
#define AIRPORT_ID_INDEX 1          // Index of airport id in program args
#define AIRPORT_INFO_INDEX 2        // Index of airport info in program args
#define AIRPORT_MAPPER_INDEX 3      // Index of airport mapper in program args
#define LOG_REQUEST "log"           // Command for control to print its log

/* Types */

/*
    summary:    Struct that contains the information to describe an airport
*/
typedef struct AirportInfo {
    char *id;                       // The id of the airport
    char *info;                     // The info for the airport
    int mapperPort;                 // The mapper port for the airport
} AirportInfo;

/*
    summary:    Struct that wraps information needed for roc connection
                handling, so it can be cast as a void*
*/
typedef struct ControlThreadWrapper {
    ArrayList *log;                 // Log of the rocs that have visted us
    sem_t *lock;                    // The semaphore guard
    int connFd;                     // Connection file descriptor
    AirportInfo info;               // Info for certain airport
} ControlThreadWrapper;

/* Functions */

void load_airport_data(AirportInfo *airportInfo, int argc, char **argv);
void handle_connections(Connection *connection, AirportInfo *airportInfo);
void * handle_roc_connection(void *args);
void process_log_command(ArrayList *log, FILE *out);
void process_roc_visiting(ArrayList *log, char *id, FILE *out,
        AirportInfo info);
int log_entry_comparator(const void *entryOne, const void *entryTwo);

#endif