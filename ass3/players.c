#include "players.h"

/**
 * Checks player arguments and runs functions needed for players to play
 * @param argc the number of arguments to this player
 * @param argv the arguments
 * @param moveLogic function pointer to player's move_logic function
 */
void run(int argc, char** argv, int (*moveLogic)(Game*, Player*)) {
    ProgramInfo info = check_args(argc, argv);
    ready();
    Game game = init_game(stdin, info.numPlayers, false);
    play(moveLogic, &game, info.id);
    finish(game);
}

/**
 * Checks arguments to a player and throws appropriate error if args are bad
 * @param argc the number of arguments
 * @param argv the arguments
 * @returns pointer to array of ints with converted string to int args
 */
ProgramInfo check_args(int argc, char** argv) {
    ProgramInfo info;
    if (argc != 3) {
        err_msg(BAD_PLAYER_ARGS);
    }
    int error;
    info.numPlayers = string_to_int(argv[1], &error);
    if (error || info.numPlayers < 1) {
        err_msg(BAD_PLAYER_COUNT);
    }
    info.id = string_to_int(argv[2], &error);
    if (error || info.id < 0 || info.id >= info.numPlayers) {
        err_msg(BAD_PLAYER_ID);
    }
    return info;
}

/**
 * Tells the dealer that this player is ready to receive path
 */
void ready(void) {
    fprintf(stdout, "^");
    fflush(stdout);
}

/**
 * Plays the game according to the way the supplied function (move_logic) plays
 * the game
 * @param move_logic the logic function for the player; different for A and B
 * @param game the game struct
 * @param player the player that the calling program is playing as
 */
void play(int (*moveLogic)(Game*, Player*), Game* game, int playerId) {
    print_path(*game, stderr);
    while(1) {
        Message message = get_message(stdin, &decode_dealer_message);
        Player* p = find_player(game, playerId, PLAYER_COM_ERROR);
        int site;
        switch(message.messageType) {
            case YT:
                site = moveLogic(game, p);
                // if player is on the last site, it shouldn't be sent
                // a YT message
                if (p->site ==
                        game->path.sites[game->path.size - 1].siteNumber) {
                    err_msg(PLAYER_COM_ERROR);
                }
                fprintf(stdout, "DO%d\n", site);
                fflush(stdout);
                break;
            case EARLY:
                finish(*game);
                break;    
            case DONE:
                distribute_extra_points(game);
                print_scores(game, stderr);
                finish(*game);
                break;
            case HAP:
                if (!is_valid_hap(message, *game)) {
                    err_msg(PLAYER_COM_ERROR);
                }
                // will need to print info of other play that isn't us.
                p = find_player(game, message.params[0], PLAYER_COM_ERROR);
                update(game, message);
                print_player_info(stderr, p);
                print_path(*game, stderr);
                break;                    
            default:
                err_msg(PLAYER_COM_ERROR);
                break;   
        }
        if (message.eof) {
            err_msg(PLAYER_COM_ERROR);
        }   
    }
}

/**
 * Updates Game and Player struct after a HAP message
 * @param game the game struct
 * @param player the player this program is playing as
 */
void update(Game* game, Message message) {
    Player* player = find_player(game, message.params[0], PLAYER_COM_ERROR);
    Site* newSite = &game->path.sites[message.params[1]];
    if(!move_player(game, player, newSite)) {
        err_msg(PLAYER_COM_ERROR); // invalid HAP 
    }
    update_player(player, game->path.sites[message.params[1]], &message);
    free(message.params);
}

/**
 * Decodes messages from the dealer
 * @param message char* format of message from dealer to player
 * Example: "DONE"
 * @returns decoded message; i.e. in the form of a Message struct
 */
Message decode_dealer_message(char* message) {
    Message decodedMessage;
    if (message == NULL) {
        err_msg(PLAYER_COM_ERROR);
    }
    decodedMessage.messageType = decode_dealer_message_type(message);
    if (decodedMessage.messageType == HAP) {
        decodedMessage.numParams = NUM_HAP_PARAMS;
        // + 3 gets rid of the 'HAP' at the start
        decodedMessage.params = decode_hap_params(message + 3);
    } else {
        decodedMessage.numParams = 0;
    }
    return decodedMessage;
}

/**
 * Gets the message type from a message from the dealer to player
 * @param message the message
 * @returns the MessageType of the message sent by dealer to player
 */ 
MessageType decode_dealer_message_type(char* message) {
    // getting string portion of message (before params)
    char word[strlen(message)];
    for (int i = 0; i <= strlen(message); i++) {
        if (isalpha(message[i])) {
            word[i] = message[i];
        } else {
            word[i] = '\0';
            break;
        }
    }
    MessageType type = -1; // an invalid type
    int expectedLenth = 0;
    if (strcmp(word, "YT") == 0) {
        type = YT;
        expectedLenth = 2; // "YT" is 2 chars  long. nO mAgIC nUmBeRS hErE!!
    } 
    if (strcmp(word, "EARLY") == 0) {
        type = EARLY;
        expectedLenth = 5; // "EARLY" is 5 chars long
    } 
    if (strcmp(word, "DONE") == 0) {
        type = DONE;
        expectedLenth = 4; // "DONE" is 4 chars long
    } 
    if (strcmp(word, "HAP") == 0) {
        return HAP; // HAP msg validity is checked differently
    }
    strtok(message, "\n"); // gets rid of trailing new line
    if (strlen(message) != expectedLenth) {
        err_msg(PLAYER_COM_ERROR);
    } else {
        return type;
    }
    return -1; // unreachable, but compiler is triggered unless its here    
}

/**
 * Decodes hap params from dealer
 * @param message message from dealer minus the first three 'HAP' characters
 * @returns int* with params in it (in order that they were specified)
 */
int* decode_hap_params(char* message) {
    strtok(message, "\n"); // gets rid of trailing new line
    int numCommas = 0;
    for (int i = 0; i < strlen(message); i++) {
        if (message[i] == ',') {
            numCommas++;
        }
    }
    if (numCommas != 4) { // a valid HAP message will have 4 commas
        err_msg(PLAYER_COM_ERROR);
    }
    int* params = (int*) malloc(sizeof(int) * NUM_HAP_PARAMS);
    char* token = strtok(message, ",");
    for (int i = 0; i < NUM_HAP_PARAMS; i++) {
        int error;
        if (i != 4) {
            params[i] = string_to_int(token, &error);
            if (error) {
                err_msg(PLAYER_COM_ERROR);
            }
        } else {
            if (token != NULL && strlen(token) == 1
                    && is_valid_card(token[0])) {
                params[4] = token[0];
            } else {
                err_msg(PLAYER_COM_ERROR);
            }
        }
        token = strtok(NULL, ",");
    }
    return params;
}

/**
 * Exits program cleanly
 * @param game main game struct
 */
void finish(Game game) {
    free_game(game);
    exit(GOOD);
}