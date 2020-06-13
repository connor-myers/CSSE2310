#include "main.h"

Dealer init_dealer(char* deckFile, char* pathFile, int numPlayers);
void setup_signal_handlers(void);
void handle_signals(int s);
void create_pipes(Dealer* dealer);
void create_players(Dealer* dealer, char** playerPrograms);
void redirect_child_fds(Dealer* dealer, int childNumber);
void send_all_players_message(Dealer* dealer, char* message);
void send_players_path(Dealer* dealer);
void play(Dealer* dealer);
bool turn(Dealer* dealer);
void update(Dealer* dealer, Player* player, Site* newSite, int* hapParams);
int find_farthest_back(Dealer* dealer);
Message decode_player_message(char* message);
int* create_hap_params(Dealer* dealer, Player* player, Site* newSite);
char* create_hap_message(int* hapParams);
void kill_all_children(Dealer* dealer);
void early_game_over(Dealer* dealer);
void free_dealer(Dealer* dealer);

Dealer* globalDealer; // pointer to main dealer, so SIGHUP handler can reap

int main(int argc, char** argv) {
    if(argc < 4) {
        err_msg(BAD_DEALER_ARGS);
    }
    Dealer dealer = init_dealer(argv[1], argv[2], argc - NUM_NON_PLAYER_ARGS);
    globalDealer = &dealer;
    setup_signal_handlers();
    create_pipes(&dealer);
    create_players(&dealer, argv + NUM_NON_PLAYER_ARGS);
    send_players_path(&dealer);
    dealer.gameStarted = true;
    play(&dealer);
    send_all_players_message(&dealer, "DONE");
    kill_all_children(&dealer);
    free_dealer(&dealer);
    return 0;
}

/**
 * Initialises main dealer struct 
 * @param deckFile the name of the deck file to be used this game
 * @param pathFile the name of the path file to be used this game
 * @param numPlayers the number of players in this game
 * @returns dealer struct for this game
 */
Dealer init_dealer(char* deckFile, char* pathFile, int numPlayers) {
    Dealer dealer;
    // loading deck
    FILE* deckStream = fopen(deckFile, "r");
    if (deckStream == NULL) {
        err_msg(BAD_DECK); // couldn't open path file
    }    
    Deck deck = init_deck(deckStream);
    fclose(deckStream);
    // loading path
    FILE* pathStream = fopen(pathFile, "r");
    if (pathStream == NULL) {
        err_msg(BAD_DEALER_PATH); // couldn't open path file
    }
    Game game = init_game(pathStream, numPlayers, true);
    rewind(pathStream); // go back to the start of the stream
    char* pathString = init_path_string(pathStream);
    fclose(pathStream);
    // dynamically allocating memory from streams/pipes
    dealer.dealerToPlayer = malloc(sizeof(int*) * numPlayers);
    dealer.playerToDealer = malloc(sizeof(int*) * numPlayers);
    for (int i = 0; i < numPlayers; i++) {
        dealer.dealerToPlayer[i] = malloc(sizeof(int) * 2); // 2 fds per pipe
        dealer.playerToDealer[i] = malloc(sizeof(int) * 2);
    }
    dealer.dealerToPlayerStreams = malloc(sizeof(FILE*) * numPlayers);
    dealer.playerToDealerStreams = malloc(sizeof(FILE*) * numPlayers);
    dealer.game = game;
    dealer.deck = deck;
    dealer.pathString = pathString;
    dealer.playerPids = malloc(sizeof(pid_t) * numPlayers);
    dealer.gameStarted = false;
    return dealer;
}

/**
 * Sets up the signal handlers
 * Code from sig.c in lecture 5.2
 */
void setup_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = handle_signals;   
    sa.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &sa, 0);
    sigaction(SIGCHLD, &sa, 0);
    sigaction(SIGPIPE, &sa, 0);
}

/**
 * Handles signals
 * @param s the signal being sent to 2310dealer
 */
void handle_signals(int s) {
    if (s == SIGHUP) {
        kill_all_children(globalDealer);
        exit(BAD_PROCESS); // exit status not checked; don't print message
    } else if (s == SIGCHLD && !globalDealer->gameStarted) {
        // a child has died before the game started properly
        kill_all_children(globalDealer);
        err_msg(BAD_PROCESS); // exit status not checked; don't print message
    } else if (s == SIGPIPE) {
        // do nothing.
    } 
}


/**
 * Creates the pipe between dealer and player processes
 * @param dealer the main dealer struct
 */
void create_pipes(Dealer* dealer) {
    for (int i = 0; i < dealer->game.numPlayers; i++) {
        if (pipe(dealer->dealerToPlayer[i]) != 0
                || pipe(dealer->playerToDealer[i]) != 0) {
            exit(BAD_PROCESS); // error creating the pipes
        }
    }
}

/**
 * Creates the player processes for the game
 * Redirects pipes and closes unnecessary ends of pipes
 * @param dealer the main dealer struct
 * @param playerPrograms the programs that will act as players
 */
void create_players(Dealer* dealer, char** playerPrograms) {
    pid_t pid;
    pid_t ppid = getpid(); // get parent id 
    for (int i = 0; i < dealer->game.numPlayers; i++) {
        pid = fork(); // should handle a pid of -1
        if (pid == 0) {
            // child
            close(dealer->dealerToPlayer[i][WRITE_END]);
            close(dealer->playerToDealer[i][READ_END]);
            redirect_child_fds(dealer, i);
            int err = execlp(playerPrograms[i], playerPrograms[i],
                    int_to_string(dealer->game.numPlayers),
                    int_to_string(i), NULL); 
            if (err == -1) {
                kill(ppid, SIGHUP);
                err_msg(BAD_PROCESS);
            }
        } else {
            // parent
            dealer->playerPids[i] = pid;
            close(dealer->dealerToPlayer[i][READ_END]);
            close(dealer->playerToDealer[i][WRITE_END]);
            // creating streams so I don't have to dup on the dealer end
            dealer->dealerToPlayerStreams[i]
                    = fdopen(dealer->dealerToPlayer[i][WRITE_END], "w");
            dealer->playerToDealerStreams[i]
                    = fdopen(dealer->playerToDealer[i][READ_END], "r");
        }
    }
}

/**
 * Redirects the file descriptors for the children to the appropriate locations
 * @param dealer the main dealer struct
 * @param childNumber the child number; what number child is this
 */
void redirect_child_fds(Dealer* dealer, int childNumber) {
    dup2(dealer->dealerToPlayer[childNumber][READ_END], STDIN_FILENO);
    dup2(dealer->playerToDealer[childNumber][WRITE_END], STDOUT_FILENO);
    int devNull = open("/dev/null", O_WRONLY); // ignore children's stderr
    dup2(devNull, STDERR_FILENO);
}

/**
* For each player, checks that it has recieved the '^' from them.
* When it has, it sends the path back to the players
* @param Dealer pointer to main dealer struct
*/
void send_players_path(Dealer* dealer) {
    for (int i = 0; i < dealer->game.numPlayers; i++) {
        int c = fgetc(dealer->playerToDealerStreams[i]);
        if (c == '^') {
            send_message(dealer->dealerToPlayerStreams[i],
                    dealer->pathString);
        } else {
            // both EOF or garbage char means bad process
            kill_all_children(dealer); // necessary?
            err_msg(BAD_PROCESS);
        }
    }
}

/**
 * Sends a message to all players
 * @param dealer pointer to the main dealer struct
 * @param message the message to be sent to all players
 */
void send_all_players_message(Dealer* dealer, char* message) {
    for (int i = 0; i < dealer->game.numPlayers; i++) {
        send_message(dealer->dealerToPlayerStreams[i], message);
    }
}

/**
 * The main game loop
 * @param dealer pointer to main dealer struct
 */
void play(Dealer* dealer) {
    print_path(dealer->game, stdout);
    bool gameOver = false;
    while(!gameOver) {
        gameOver = turn(dealer);
    }
    distribute_extra_points(&dealer->game);
    print_scores(&dealer->game, stdout);
}

/**
 * Runs a single turn of the game
 * @param dealer pointer to main dealer struct
 * @returns true if the game is over and thus a valid move could not be made;
 * false otherwise.
 * Note: If the game ends as a result of a move being made
 * the function will still return false. It will return true the next time
 * turn() is called.
 */
bool turn(Dealer* dealer) {
    // player id of the player's whose turn it is
    int id = find_farthest_back(dealer); 
    if (id == -1) {
        return true; // the game is over
    }
    // request input from player
    send_message(dealer->dealerToPlayerStreams[id], "YT");
    // get move from player
    Message move = get_message(dealer->playerToDealerStreams[id],
            &decode_player_message);
    if (move.error) {
        early_game_over(dealer);
    }
    // find the player struct from id
    Player* p = find_player(&dealer->game, id, DEALER_COM_ERROR);
    Site* s = &dealer->game.path.sites[move.params[0]];
    int* hapParams = create_hap_params(dealer, p, s);
    // update game and player information
    update(dealer, p, s, hapParams);    
    // create HAP message and send it to all players
    char* hapMsg = create_hap_message(hapParams);
    send_all_players_message(dealer, hapMsg);
    print_player_info(stdout, p);
    print_path(dealer->game, stdout);
    if (move.eof) {
        // if we find EOF, we process the message
        // and then exit with early game over
        early_game_over(dealer);
    }
    free(move.params);
    free(hapMsg);
    free(hapParams);
    return false;
}

/**
 * Updates the sate of the game and the player after a move occurs
 * @param dealer pointer to main dealer struct
 * @param player pointer to the player to be updated
 * @param newSite pointer to the new site the player arrived at
 * @param hapParams "array" with the parameters of the HAP message
 */
void update(Dealer* dealer, Player* player, Site* newSite, int* hapParams) {
    // move player (i.e. update the game)
    if (!move_player(&dealer->game, player, newSite)) {
        early_game_over(dealer);
    }
    // creating a hap msg ourselves so we can use same player update code
    // as the players. Looks pretty cringe, but saves me some time.
    Message hapMsg;
    hapMsg.eof = false;
    hapMsg.messageType = HAP;
    hapMsg.numParams = NUM_HAP_PARAMS;
    hapMsg.params = hapParams;
    update_player(player, *newSite, &hapMsg);
}

/**
 * Finds the player who is farthest back on the path
 * Note: The player farthest back is the player whose turn it is.
 * @param dealer pointer to main dealer struct
 * @returns playerId of the player farthest back on the path 
*/
int find_farthest_back(Dealer* dealer) {
    // size - 1 so we don't check the final barrier
    for (int i = 0; i < dealer->game.path.size - 1; i++) {
        if (dealer->game.path.sites[i].numPlayers > 0) {
            return dealer->game.path.sites[i].
                    playerIds[dealer->game.path.sites[i].numPlayers - 1];
        }
    }
    return -1; // -1 means all players have reached barrier 
}

/**
 * Decodes a player message to the dealer; i.e. converts char* to Message
 * @param message the char* representation of a message to the dealer
 * @returns char* message converted into a Message struct format
 */
Message decode_player_message(char* message) {
    Message msg;
    msg.error = false;
    if (strlen(message) < 2) {
        // < 3 because 'D' + 'O' + n (at least 1 digit)
        // not necessarilly '\n' terminated, might be EOF terminated
        msg.error = true;
        return msg;
    }
    if (message[0] != 'D' || message[1] != 'O') {
        msg.error = true;
        return msg;
    }
    msg.messageType = DO;
    // if not EOF terminated, there should be a '\n' on end
    // must be removed for proper parsing
    if (message[strlen(message) - 1] == '\n') {
        message = strtok(message, "\n"); // remove trailing new line char
    }
    int error;
    int site = string_to_int(message + 2, &error); // + 2 removes 'DO'
    if (error) {
        msg.error = true;
    }
    msg.numParams = 1;
    msg.params = malloc(sizeof(int) * msg.numParams);
    msg.params[0] = site;
    return msg;
}

/**
 * Creates an integer "array" with the hap parameters (in order)
 * @param dealer pointer to main dealer struct
 * @param player pointer to the player which made the move 
 * @param newSite pointer to the site the player moved to
 * @returns int "array" with the parameters for a HAP message representing
 * the move just made
 */
int* create_hap_params(Dealer* dealer, Player* player, Site* newSite) {
    int* hapParams = malloc(sizeof(int) * NUM_HAP_PARAMS);
    int dPoints = 0; // change in points;
    int dMoney = 0; // change in money
    char cardDrawn = '0'; // the card drawn
    switch (newSite->type) {
        case MO_SITE:
            dMoney = 3;
            break;
        case DO_SITE:
            dMoney = -1 * player->money;
            dPoints = player->money / 2;
            break;
        case RI_SITE:
            cardDrawn = dealer->deck.values[dealer->deck.count
                    % dealer->deck.size] - 16; // 16 is diff between 'A' & '1'
            dealer->deck.count++;
            break;
        default:
            break;    
    }
    hapParams[0] = player->playerId;
    hapParams[1] = newSite->siteNumber;
    hapParams[2] = dPoints;
    hapParams[3] = dMoney;
    hapParams[4] = cardDrawn;
    return hapParams;
}

/**
 * Creates the HAP message to be sent to the players.
 * Note: Dealer reuses code from players, so it actually updates it's state
 * with the HAP message as well just like players
 * @param hapParams the paramaters in the HAP message
 * @returns String HAP message to be sent to players 
 */
char* create_hap_message(int* hapParams) {
    char* hapMsg = malloc(sizeof(char) * MAX_HAP_LEN); // long enough
    sprintf(hapMsg, "HAP%d,%d,%d,%d,%c", hapParams[0],
            hapParams[1], hapParams[2], hapParams[3], hapParams[4]);
    return hapMsg;
}

/**
 * Kills child processes
 * @param dealer pointer to main dealer struct
 */
void kill_all_children(Dealer* dealer) {
    for (int i = 0; i < dealer->game.numPlayers; i++) {
        int status = 0;
        waitpid(dealer->playerPids[i], &status, WNOHANG);
        if (!WIFEXITED(status)) {
            // child isn't dead; we need to kill it.
            kill(dealer->playerPids[i], SIGKILL);
        }
    }
}

/**
 * Handles early game over
 * i.e. sends "EARLY" message to players and exits with DEALER COMM ERROR
 * @param dealer pointer to main dealer struct
 */
void early_game_over(Dealer* dealer) {
    send_all_players_message(dealer, "EARLY");
    err_msg(DEALER_COM_ERROR);
}

/**
 * Frees all memory the main dealer struct dynamically allocated
 * @param dealer pointer to main dealer struct
 */
void free_dealer(Dealer* dealer) {
    free_game(dealer->game);    
    free(dealer->deck.values);
    for (int i = 0; i < dealer->game.numPlayers; i++) {
        fclose(dealer->dealerToPlayerStreams[i]);
        fclose(dealer->playerToDealerStreams[i]);
        free(dealer->dealerToPlayer[i]);
        free(dealer->playerToDealer[i]);
    }
    free(dealer->dealerToPlayer);
    free(dealer->playerToDealer);
    free(dealer->dealerToPlayerStreams);
    free(dealer->playerToDealerStreams); 
    free(dealer->pathString);
    free(dealer->playerPids);
}
