#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>

typedef enum MessageType {
    YT = 0, // Represents a "Your Turn" message from dealer to player
    DO = 1, // Represents a "DOn" message from player to dealer
    EARLY = 2, // Reprents an early game over message from dealer to player
    DONE = 3, // Represents a normal game over message from dealer to player
    HAP = 4 // Represents a "Happening" message from dealer to all players
} MessageType;

typedef struct Message {
    MessageType messageType; // the type of message this is
    int numParams; // the number of extra params this message has
    int* params; // the params for this message
    bool eof; // if EOF was encountered in this message
    bool error; // true iff error in message that would result in comm error
} Message;

#endif