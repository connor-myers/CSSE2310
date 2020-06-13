#include "mapper2310.h"

int main(int argc, char **argv)
{
    // ensure we handle SIGINT properly 
    setup_signal_handlers();

    // Finding a port for this mapper to listen on
    Connection connection;
    listen_to_port(&connection);

    // printing what port this mapper is listening on
    printf("%d\n", connection.port);
    fflush(stdout);

    // initialising necessary variables
    ArrayList map;
    arraylist_init(&map, sizeof(Mapping *));
    MapperThreadWrapper params[MAX_CONNECTIONS];
    pthread_t threads[MAX_CONNECTIONS];
    int numThreads = INITIAL_THREAD_COUNT; // initially, there are 0 threads
    sem_t lock;
    init_lock(&lock);

    // Spawning threads to handle connections to this mapper
    while (true) {
        int connFd = accept(connection.server, 0, 0);
        if (connFd <= MIN_FILE_DESCRIPTOR) {
            continue;
        }

        // wrappping required parameters into wrapping struct 
        MapperThreadWrapper param = {&map, &lock, connFd};
        params[numThreads] = param;
        pthread_create(&(threads[numThreads]), NULL,
                handle_mapper_connection, (void *) &(params[numThreads])); 
        numThreads++; 
    }
        
    return GOOD_MAPPER_EXIT;
}

/*  
    summary:            Handles control2310s connecting to us

    args:               MapperThreadWrapper* cast as a void*

    returns:            The NULL pointer
*/
void *handle_mapper_connection(void *args)
{
    // unwrapping the parameters 
    MapperThreadWrapper *params = (MapperThreadWrapper*) args;

    // Creating streams for easier communication
    int connFdCopy = dup(params->connFd);
    FILE *in = fdopen(params->connFd, READ);
    FILE *out = fdopen(connFdCopy, WRITE);
    handle_input(in, out, params->map, params->lock);

    return NULL;
}

/*
    summary:            Handles input from control2310 to mapper2310

    in:                 The input stream for comms with control

    out:                The output stream for comms with control

    map:                Pointer to the map for this Mapper

    lock:               Pointer to the semaphore lock/guard for this Mapper
*/
void handle_input(FILE *in, FILE *out, ArrayList *map, sem_t *lock)
{
    char *message;
    while (message = get_line(in), message != NULL) {
        take_lock(lock); // reserve access to the map

        /*
            Indexing message at 0 gives us the first character of the
            message which we will use to determine what function to redirect
            the message to.
        */
        switch (message[0]) {
            case GET_PORT:
                handle_get_port_command(message, out, map);
                break;
            case ADD_MAPPING:
                handle_add_mapping_command(message, out, map);
                break;
            case PRINT_MAP:
                handle_display_map_command(message, out, map);
                break;
            default:
                break;
        }
        
        release_lock(lock); // release access to the map
    }
}

/*
    summary:            Handles the "?ID" command from control
                        I.e. Finds Mapping with specified id and
                        sends the port to requesting control.
                        However, message may just be "?" which is invalid.
                        This function checks for that and ignores
                        any invalid messages.

    message:            The message string sent by control

    out:                Output stream to the control

    map:                Pointer to map for this Mapper
*/
void handle_get_port_command(char *message, FILE *out, ArrayList *map)
{
    int port = INVALID_PORT;

    if (has_invalid_chars(message) || strlen(message) < MIN_GET_PORT_LENGTH) {
        return;
    }

    // search the map for the id specified by the requesting control
    char *id = message + 1; // the + 1 gets rid of the '?' at start of message
    for (int i = 0; i < map->size; i++) {
        Mapping *mapping = (Mapping *) map->entries[i];
        if (strcmp(mapping->id, id) == 0) {
            port = mapping->port;            
        }
    }

    // This means ID was not found in the map 
    if (port == INVALID_PORT) {
        fprintf(out, ";\n");
        fflush(out);
        return;
    }

    // send the port number to the roc
    fprintf(out, "%d\n", port);
    fflush(out);
}

/*  
    summary:            Handles the "!ID:PORT" command from
                        control2310
                        I.e. Adds a mapping of (id,port) to the map

    message:            The message string sent by control

    out:                Output stream to the control

    map:                Pointer to map that stores Mappings for this M
*/
void handle_add_mapping_command(char *message, FILE *out, ArrayList *map)
{
    if (num_char_in_string(message, MAP_DELIM) != VALID_NUM_COLONS) {
        return;
    }

    char *token; // for strtok_r
    /*
        The + 1 in "message + 1" below removes the '!' at the start
        of the message
    */
    char *id = strtok_r(message + 1, MAP_DELIM_AS_STRING, &token); 
    char *port = strtok_r(NULL, MAP_DELIM_AS_STRING, &token);

    /*
        If the portString is still NULL,
        it means there was no ':' (i.e. MAPPING_DELIMITER) in the message
    */
    if (port == NULL) {
        return;
    }

    StringConErr error = false;
    int convertedPort = string_to_int(port, &error); // int version of port
    if (error != FINE || !is_id_unique(id, map)
            || !is_valid_port(convertedPort)) {
        return;        
    }

    // mapping needs to be dynamically allocated
    Mapping *mapping = malloc(sizeof(Mapping));
    mapping->id = id;
    mapping->port = convertedPort;

    // adding the new mapping
    ListError listError = false;
    arraylist_add(map, (void *) mapping, &listError);
    if (listError != NO_ERROR) {
        // bad; for debugging
    }

    return;
}

/*
    summary:            Sorts the map into lexographic order
                        before printing map over output stream

    out:                Stream to print map out to

    map:                Pointer to the map
*/
void handle_display_map_command(char *message, FILE *out, ArrayList *map)
{
    // ensuring message is correct format
    if (strlen(message) != VALID_DISPLAY_COMMAND_LENGTH) {
        return;
    }

    // sorting the map
    qsort((void*) map->entries, map->size, sizeof(Mapping *),
            &mapping_comparator);

    // printing the map
    for (int i = 0; i < map->size; i++) {
        Mapping *entry = (Mapping*) map->entries[i];
        fprintf(out, "%s:%d\n", entry->id, entry->port);
    }
    fflush(out);

    return;        
}

/*
    summary:            Looks through the map to see if the
                        inputted id is unique.
                        (i.e. it's not already in the map)

    id:                 The id to search the map for

    map:                Pointer to the map to search

    Returns:            True iff the id is not already in the map
*/
bool is_id_unique(char *id, ArrayList *map)
{
    for (int i = 0; i < map->size; i++) {
        Mapping *mapping = (Mapping *) map->entries[i];
        if (strcmp(mapping->id, id) == 0) {
            return false;
        }
    }

    return true;
}

/*  
    summary:            Compares two mapping's ids
                        Returns the lexiographic difference

    mappingOne:         Mapping* cast as a void*'

    mappingTwo:         Mapping* cast as a void* 

    returns:        > 0 if one is lexographically > two
                    0 if one is lexographically = two
                    < 0 if one is lexographically < two
*/
int mapping_comparator(const void *mappingOne, const void *mappingTwo)
{
    const Mapping *unwrappedMappingOne = *(const Mapping **) mappingOne;
    const Mapping *unwrappedMappingTwo = *(const Mapping **) mappingTwo;
    return strcmp(unwrappedMappingOne->id, unwrappedMappingTwo->id);
}