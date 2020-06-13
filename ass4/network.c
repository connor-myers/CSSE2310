#include "network.h"

/*
    summary:            Connects to any available ephemeral port, initialises
                        values inside connection struct to specify socket
                        address and port number.
                        This function can terminate calling program
                        with exit code CONNECTION_ERROR if there is a problem
                        with any of the system calls.

    connection:         Pointer to connection struct; will be filled with
                        details about the port being listen upon

    Note:               This function is basically 100% taken from lecture code
*/
void listen_to_port(Connection *connection)
{
    // set-up
    struct addrinfo *ai = EMPTY;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;        // IPv4 for generic could use AF_UNSPEC
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  // Because we want to bind with it    

    // try to connect to an any available ephemeral port
    int err;
    if ((err = getaddrinfo("localhost", 0, &hints, &ai))) { 
        freeaddrinfo(ai);
        fprintf(stderr, "%s\n", gai_strerror(err));
        exit(CONNECTION_ERROR);  // could not work out the address
    }

    // create a socket and bind it to a port
    int serv = socket(AF_INET, SOCK_STREAM, 0); // 0 == use default protocol
    if (bind(serv, (struct sockaddr *) ai->ai_addr, sizeof(struct sockaddr))) {
        perror("Binding");
        exit(CONNECTION_ERROR);
    }

    // Which port did we get?
    struct sockaddr_in ad;
    memset(&ad, 0, sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr_in);
    if (getsockname(serv, (struct sockaddr *) &ad, &len)) {
        perror("sockname");
        exit(CONNECTION_ERROR);
    }

    // listen on connection
    if (listen(serv, MAX_CONNECTIONS)) { 
        perror("Listen");
        exit(CONNECTION_ERROR);
    }

    connection->port = ntohs(ad.sin_port);
    connection->server = serv;
}

/*
    summary:            Connect to the specified port on localhost
                        If successful, returns SocketStreams containing
                        in and out streams for that connections.
                        If there is a failure, both in and out streams
                        in returned SocketStreams will be NULL

    port:               The port to connect to     

    returns:            SocketStreams (i.e. an in and out stream) on
                        specified port. In and out streams are both NULL
                        if there were any problems in converting or opening
                        streams.                          
*/
SocketStreams connect_to_port(int port)
{
    // converting int port to string
    char *convertedPort = int_to_string(port);
    
    // initialising variables
    SocketStreams streams = {NULL, NULL};
    struct addrinfo *ai = EMPTY;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4  for generic could use AF_UNSPEC
    hints.ai_socktype = SOCK_STREAM;
    int err;

    // finding out information for port we're trying to connect to
    if ((err = getaddrinfo("localhost", convertedPort, &hints, &ai))) {
        freeaddrinfo(ai);
        fprintf(stderr, "%s\n", gai_strerror(err));
        return streams;   // could not work out the address
    }

    // creating socket to communicate on 
    int fd = socket(AF_INET, SOCK_STREAM, 0); // 0 == use default protocol
    if (connect(fd, (struct sockaddr *) ai->ai_addr,
            sizeof(struct sockaddr))) {
        return streams;
    }

    // creating streams to communicate over
    int fd2 = dup(fd);
    streams.in = fdopen(fd, READ);
    streams.out = fdopen(fd2, WRITE);
    return streams;
}

/*
    summary:            Determines if input port is valid or not
                        I.e. More than 0 and less than 65535

    port:               The port number to determine the validity of

    Returns:            True iff the port is valid
*/
bool is_valid_port(int port) {
    return (port >= MIN_PORT_NUMBER && port <= MAX_PORT_NUMBER);
}