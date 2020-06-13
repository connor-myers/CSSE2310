#include "control2310.h"

int main(int argc, char **argv)
{
    // Ensure we handle SIGINT properly 
    setup_signal_handlers();
    
    // Checking args
    if (argc != MIN_CONTROL_ARGS && argc != MAX_CONTROL_ARGS) {
        err_msg(BAD_CONTROL_ARGS);
    }
    
    // Loading airport data
    AirportInfo airport; 
    load_airport_data(&airport, argc, argv);

    // Listen on ephemeral port and tell everyone what it is on stdout
    Connection connection;
    listen_to_port(&connection);
    printf("%d\n", connection.port);
    fflush(stdout);

    // If mapper port was specified, tell mapper our id and port we're using
    if (airport.mapperPort != NO_MAPPER) {
        SocketStreams streams = connect_to_port(airport.mapperPort);
        if (streams.in == NULL || streams.out == NULL) {
            err_msg(BAD_PORT);
        }
        fprintf(streams.out, "!%s:%d\n", airport.id, connection.port);
        fflush(streams.out);
        fclose(streams.out); // closing the connection to mapper
        fclose(streams.in);
    }

    // Accept connections from Rocs and act on them
    handle_connections(&connection, &airport);

    return GOOD;
}

/*
    summary:            Loads the data for the airport
                        I.e. the id, info and mapper (if required)
                        This function can exit the program
                        if there are problems with args

    airport:            Pointer to empty airport struct to be filled

    argc:               Number of arguments into program

    argv:               The arguments into the program
*/
void load_airport_data(AirportInfo *airportInfo, int argc, char **argv)
{
    // extract and validate the id 
    if (has_invalid_chars(argv[AIRPORT_ID_INDEX])) {
        err_msg(BAD_CHARS);
    }    
    airportInfo->id = argv[AIRPORT_ID_INDEX];

    // extract and validate the info
    if (has_invalid_chars(argv[AIRPORT_INFO_INDEX])) {
        err_msg(BAD_CHARS);
    }    
    airportInfo->info = argv[AIRPORT_INFO_INDEX];

    // extract and validate the mapper
    if (argc == MAX_CONTROL_ARGS) {
        StringConErr error = FINE;
        int port = string_to_int(argv[AIRPORT_MAPPER_INDEX], &error);
        if (error == FINE && is_valid_port(port)) {
            airportInfo->mapperPort = port;
        } else {
            err_msg(BAD_PORT_NUMBER);
        }        
    } else {
        airportInfo->mapperPort = NO_MAPPER;
    }
}

/*
    summary:            Handles connections from rocs
                        Spawns a new thread for each roc connection

    connection:         The connection information for this control
                        I.e. The port and socket this control is listening on

    airportInfo:        Pointer to AirportInfo struct which contains all the
                        relevant information for this control
*/
void handle_connections(Connection *connection, AirportInfo *airportInfo)
{
    // initialising necessary variables
    ArrayList log;
    arraylist_init(&log, sizeof(char *));
    ControlThreadWrapper params[MAX_CONNECTIONS];
    pthread_t threads[MAX_CONNECTIONS];
    int numThreads = INITIAL_THREAD_COUNT;
    sem_t lock;
    init_lock(&lock);

    // Spawning threads to handle connections to this mapper
    while (true) {
        int connFd = accept(connection->server, 0, 0);
        if (connFd <= MIN_FILE_DESCRIPTOR) {
            continue;
        }

        ControlThreadWrapper param = {&log, &lock, connFd, *airportInfo};
        params[numThreads] = param;
        pthread_create(&(threads[numThreads]), NULL,
                handle_roc_connection, (void *) &(params[numThreads])); 
        numThreads++; 
    }
}

/* 
    summary:            Handles a connection from a roc to us (a control2310)
                        Takes the message from roc and redirects it to the
                        function which can handle that message appropriately

    args:               ControlThreadWrapper struct cast as a void*

    returns:            The NULL pointer
*/
void *handle_roc_connection(void *args)
{
    // unwrapping parameters
    ControlThreadWrapper *params = (ControlThreadWrapper *) args;

    // creating streams for easy communication
    int connFdCopy = dup(params->connFd);
    FILE *in = fdopen(params->connFd, READ);
    FILE *out = fdopen(connFdCopy, WRITE);

    char *message;
    while (message = get_line(in), message != NULL) {
        take_lock(params->lock); // reserve access to read/write log
        
        // redirect message to appropriate function to handle it
        if (strcmp(message, LOG_REQUEST) == 0) {
            process_log_command(params->log, out);
            fclose(in); // close the connection after we process log    
            release_lock(params->lock);
            return NULL;
        } else {
            process_roc_visiting(params->log, message, out, params->info);
        }

        release_lock(params->lock); // release access to read/write log  
    }

    return NULL;
}

/*
    summary:            Handles "log" message from roc to us (a control)
                        This means printing our log (in lexographic order)
                        on the stream that sent the message, followed by ".\n"

    log:                Pointer to log, storing all the rocs that visited us

    out:                Write stream for the connection that requested log
*/
void process_log_command(ArrayList *log, FILE *out)
{
    // sort the log
    qsort((void *) log->entries, log->size, sizeof(char*),
            &log_entry_comparator);    

    // print the log
    for (int i = 0; i < log->size; i++) {
        fprintf(out, "%s\n", (char*) log->entries[i]);
    }
    fprintf(out, ".\n");
    fflush(out);
}

/*
    summary:            Handles a roc visting this control
                        This means, storing the id the roc sends us into our
                        log and sending the roc our (this control's) info

    log:                Pointer to log containing the ids of visiting rocs

    id:                 The id to add to our log. 

    out:                The output stream to the roc

*/
void process_roc_visiting(ArrayList *log, char *id, FILE *out,
        AirportInfo info)
{
    if (has_invalid_chars(id)) {
        return; // ignore a roc that sends us an id with invalid chars
    }

    // add the id to our log
    if (strlen(id) == 0) {
        return;
    }
    ListError error = NO_ERROR;
    arraylist_add(log, (void*) id, &error);
    if (error != NO_ERROR) {
        return; // should never happen in practice; for debugging 
    }

    // send the roc our info
    fprintf(out, "%s\n", info.info);
    fflush(out);
}

/*
    Glorified wrapping function for strcmp since qsort comparator
    Must take in const void* as parameters.

    Compares entryOne and entryTwo just like strcmp does 

    entryOne: char* cast as const void*
    entryTwo: char* cast as const void*    

    returns: < 0 if entryOne is lexographically less than entryTwo
             = 0 if entryOne is lexographically equal to entryTwo
             > 0 if entryOne is lexographically more than entryTwo

*/
int log_entry_comparator(const void *entryOne, const void *entryTwo)
{
    const char *unwrappedEntryOne = *(const char**) entryOne;
    const char *unwrappedEntryTwo = *(const char**) entryTwo;
    return strcmp(unwrappedEntryOne, unwrappedEntryTwo);
}
