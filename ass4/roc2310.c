#include "roc2310.h"

int main(int argc, char **argv)
{
    RocInfo info; // holds info on this roc
    extract_roc_info(&info, argc, argv);

    // info.destinationPorts will be set now
    check_destinations(&info);

    // setting up our log for control infos
    ArrayList log;
    arraylist_init(&log, sizeof(char *));

    bool problem = false;
    visit_destinations(&info, &log, &problem);

    // print log to stdout
    print_log(&log);

    // we failed to connect to at least one destination; throw error
    if (problem) {
        err_msg(BAD_MAP_CONNECTIONS);
    }
    
    return GOOD;
}

/*
    summary:            Extracts the information from program arguments
                        Checks if args were valid or not; exits program with
                        appropriate exit code if not valid.
                        Modifies RocInfo to contain relevant information

    info:               Pointer to RocInfo; to be filled with info from args 

    argc:               The number of arguments into this program

    argv:               The arguments inputted to this program               
*/
void extract_roc_info(RocInfo *info, int argc, char **argv)
{
    if (argc < MIN_ROC_ARGS) {
        err_msg(BAD_ROC_ARGS);
    }
    char *planeID = argv[PLANE_ID_INDEX];
    char *mapper = argv[MAPPER_PORT_INDEX]; // mapper port in string form

    if (has_invalid_chars(planeID)) {
        err_msg(BAD_ROC_ARGS); // Not explicitly stated in spec
    }

    // planeId is valid if we reach this point
    info->planeId = planeID;

    StringConErr error = false;
    int port = string_to_int(mapper, &error);
    if (strcmp(mapper, NO_MAPPER_INPUT) != 0 && !is_valid_port(port)) {
        err_msg(BAD_MAPPER_PORT);
    }

    if (strcmp(mapper, NO_MAPPER_INPUT) == 0) {
        port = INVALID_PORT;
    }
    
    // mapper port number is valid if we reach this point
    info->mapperPort = port;

    if (argc > MIN_ROC_ARGS) {
        info->destinations = argv + NUM_NON_DESTINATION_ARGS;        
    } else {
        info->destinations = NULL;
    }

    // Will be zero if no destinations were entered
    info->numDestinations = argc - NUM_NON_DESTINATION_ARGS;
}

/*
    summary:            Checks the destinations to see if a mapper is required.
                        If it's found that a mapper is required (i.e. an id
                        was specified, not a port), asks mapper for port.
                        If port not given, sets that destination to NULL
                        pointer. This function can terminate the program with
                        MAPPER_REQUIRED or INVALID_MAPPER or BAD_MAP_ENTRIES 
                        exit statuses

    info:               Pointer to roc info;
                        all information about this plane                
*/
void check_destinations(RocInfo *info)
{
    // connect to the mapper, we might need it
    SocketStreams streams = {NULL, NULL};
    if (info->mapperPort != MAPPER_NOT_REQUIRED) {
        streams = connect_to_port(info->mapperPort);
        if (streams.in == NULL || streams.out == NULL) {
            err_msg(INVALID_MAPPER);
        }
    }

    info->destinationPorts = calloc(info->numDestinations, sizeof(int));
    for (int i = 0; i < info->numDestinations; i++) {
        StringConErr error = false;
        int port = string_to_int(info->destinations[i], &error);
        if (error != FINE && !is_valid_port(port)) {
            // mapper required; this destination is an id
            if (info->mapperPort == MAPPER_NOT_REQUIRED) {
                err_msg(MAPPER_REQUIRED);
            }
            get_port_from_mapper(&port, info->destinations[i], streams);   
        }
        info->destinationPorts[i] = port;                     
    }
}

/*
    summary:            When one of the destinations provided is an id
                        and not a port, this function asks the mapper
                        what the port a control is listening on.
                        Can terminate the program with
                        BAD_MAP_ENTRIES exit code
                    
    port:               Pointer to int where value of port
                        should be set

    id:                 The id of the control

    streams:            The network communcation channel
                        with the mapper                

*/
void get_port_from_mapper(int *port, char *id, SocketStreams streams)
{
    // request the port
    fprintf(streams.out, "?%s\n", id);
    fflush(streams.out);

    // receive the port
    char *response = get_line(streams.in);
    if (response == NULL || strcmp(response, NO_MAPPER_ENTRY) == 0) {
        err_msg(BAD_MAP_ENTRIES); // mapper had no entry for that id
    }

    // Checking if what we received from mapper is a valid port number
    StringConErr error = false;    
    *port = string_to_int(response, &error);
    if (error != FINE || !is_valid_port(*port)) {
        *port = INVALID_PORT; // don't try to connect to this
    }
}

/* 
    summary:            Visits the destinations desribed in
                        info.destinationPorts. 
                        (I.e. it sends those controls our id, 
                        and logs control's info).
                        This function can terminate the program with
                        BAD_MAP_CONNECTIONS exit code.

    info:               Pointer to the RocInfo that contains this plane's info

    log:                Pointer to our control info log  

    problem:            Pointer to bool. Set to true iff there was an issue 
                        connecting to at least one destination              
*/
void visit_destinations(RocInfo *info, ArrayList *log, bool *problem)
{
    // will be set to true if we couldn't connect to a destination 
    *problem = false;
    for (int i = 0; i < info->numDestinations; i++) {
        bool error = visit_destination(info->destinationPorts[i], log, info);
        if (error) {
            *problem = true;
        }        
    }
}

/*
    summary:            Attempts to visit a single destination port
                        If we can connect, send id and add info to log
                        Returns true iff there was an error connection

    port:               Port of the destination we're trying to connect to

    log:                Pointer to our control info log

    info:               Pointer to our RocInfo struct        
*/
bool visit_destination(int port, ArrayList *log, RocInfo *info)
{
    // earlier function marked bad ports as INVALID_PORT
    if (port == INVALID_PORT) {
        return true;
    }

    SocketStreams streams = connect_to_port(port);
    if (streams.in == NULL || streams.out == NULL) {
        return true; // couldn't connect 
    }

    // we are connected. Send control our id and retrieve their info
    fprintf(streams.out, "%s\n", info->planeId);
    fflush(streams.out);
    char *controlInfo = get_line(streams.in);

    if (controlInfo == NULL || strlen(controlInfo) == 0
            || has_invalid_chars(controlInfo)) {
        /*
            If this is sent, control sent us garbage.
            Spec doesn't say how to handle this.
        */
    }

    // add info to our log
    ListError error = NO_ERROR;
    arraylist_add(log, controlInfo, &error);
    if (error != NO_ERROR) {
        // would never happen
    }

    return false;
}

/*
    summary:            Prints out the log of airport infos
                        this roc has visited.

    log:                ArrayList pointer to the log with the control
                        infos stored in it.                
*/
void print_log(ArrayList *log)
{
    for (int i = 0; i < log->size; i++) {
        char *info = (char *) log->entries[i];
        printf("%s\n", info);
        fflush(stdout);
    }
}