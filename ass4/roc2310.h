#ifndef ROC2310_H
#define ROC2310_H

#include "utility.h"
#include "exit.h"
#include "arrayList.h"
#include "network.h"

/* Definitions */

#define MAPPER_NOT_REQUIRED -1      // mapper is not required for destinations
#define NO_MAPPER_INPUT "-"         // Represents user did not input a mapper
#define NO_MAPPER_ENTRY ";"         // Represents mapper could not find id
#define MIN_ROC_ARGS 3              // the min num of roc args for valid call
#define PLANE_ID_INDEX 1            // index of the plane id in argv
#define MAPPER_PORT_INDEX 2         // index of the mapper port in argv
#define NUM_NON_DESTINATION_ARGS 3  // number of args which aren't destinations

/* Types */

/*
    summary:    Struct containing all the information about a Roc (Aeroplane)
*/
typedef struct RocInfo {
    char *planeId;          // the planeId for the roc
    int mapperPort;         // the port for the mapper (or MAPPER_NOT_REQUIRED)
    char **destinations;    // the destinations for this roc to visit
    int *destinationPorts;  // port numbers for the destinations
    int numDestinations;    // the number of destinations
} RocInfo;

/* Functions */

void extract_roc_info(RocInfo *info, int argc, char **argv);
void check_destinations(RocInfo *info);
void get_port_from_mapper(int *port, char *id, SocketStreams streams);
void visit_destinations(RocInfo *info, ArrayList *log, bool *problem);
bool visit_destination(int port, ArrayList *log, RocInfo *info);
void print_log(ArrayList *log);

#endif