#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "message.h"
#include "errs.h"
#include "utility.h"
#include "constants.h"

Message get_message(FILE* stream, Message (*decode)(char*));
void send_message(FILE* stream, char* message);

#endif