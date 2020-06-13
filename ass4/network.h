#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "utility.h"

/* Definitions */

#define CONNECTION_ERROR 11         // General connection error. For debugging.
#define MIN_PORT_NUMBER 1           // Minimum valid port number
#define MAX_PORT_NUMBER 65535       // Maximimum valid port number
#define MAX_CONNECTIONS 1024        // Maximum number of connections allowed
#define INVALID_PORT -1             // An invalid port
#define INITIAL_THREAD_COUNT 0      // Initially there are 0 threads
#define MIN_FILE_DESCRIPTOR 0       // The minimum value for a valid a fd
#define EMPTY 0                     // used to declare addrinfo as empty

/* Types */

/*
    summary:    represents the connection that a program is listening on
*/
typedef struct Connection {
    int server; // the server for the connection
    in_port_t port; // the port the connection was made on
} Connection;

/*
    summary:    represents the communication channel between two programs on
                a network.
*/
typedef struct SocketStreams {
    FILE *in;                       // stream for reading data into network
    FILE *out;                      // stream for writing data from network
} SocketStreams;

/* Functions */

void listen_to_port(Connection *connection);
SocketStreams connect_to_port(int port);
bool is_valid_port(int port);

#endif