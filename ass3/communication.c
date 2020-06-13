#include "communication.h"

/**
 * Gets a decoded message from the indicated stream
 * @param stream the stream to get the message from
 * @param decode function pointer to the function to be used to
 * decode the message; decode function must take in char* and return Message
 * @returns decoded message from indicated stream
 */
Message get_message(FILE* stream, Message (*decode)(char*)) {
    Message decodedMessage;
    bool eof; // set to true if EOF encountered, false otherwise
    char* message = get_line(stream, &eof);
    decodedMessage = decode(message);
    decodedMessage.eof = eof;
    free(message);
    return decodedMessage;
}

/**
 * Sends a message to the stream indicated.
 * Note, this function adds the new line character to the message.
 * Do not add the new line to message before hand
 * @param stream the stream to send the message to
 * @param message the message (without new line)
 */
void send_message(FILE* stream, char* message) {
    fprintf(stream, "%s\n", message);
    fflush(stream);
}