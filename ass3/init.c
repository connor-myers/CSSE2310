#include "init.h"

// private functions
Path init_path(FILE* pathStream, int numPlayers, bool dealer);
int get_path_size(char* pathString, int colonIndex, Err err);
int get_size(FILE* pathFile, Err error);
Site* get_path_sites(char* pathString, int size, int numPlayers, Err err);
Player* init_players(int numPlayers);
Site string_to_site(char* site, Err err, int numPlayers);
void place_initial_players(Path* path, int numPlayers);

/**
 * Initialises the game
 * @param pathStream the stream where the path is going to come in from
 * @param numPlayers the number of players in the game
 * @param dealer true iff the dealer is calling the function, false for players
 * @returns initialised Game struct
 */
Game init_game(FILE* pathStream, int numPlayers, bool dealer) {
    Game game;
    game.numPlayers = numPlayers;
    game.path = init_path(pathStream, numPlayers, dealer);
    game.players = init_players(numPlayers);
    return game;
}

/**
 * Reads in the path from pathStream and returns Path struct representing
 * that inputted path
 * @param pathStream the stream the path is coming in from
 * @param numPlayers the number of players in this game
 * @param dealer true if dealer is calling, false if player is called 
 * (This is done so correct error is thrown depending on who calls function)
 * @returns Path struct representing string version of path in pathStream 
 */
Path init_path(FILE* pathStream, int numPlayers, bool dealer) {
    Path path;
    Err err = dealer ? BAD_DEALER_PATH : BAD_PLAYER_PATH;
    char* pathString = init_path_string(pathStream);
    // ensuring no trailing characters when dealer reading path from file
    if (dealer && fgetc(pathStream) != EOF) {
        err_msg(BAD_DEALER_PATH);
    }
    int colonIndex = -1; // the index in the path string where the ; is
    for (int i = 0; i < strlen(pathString); i++) {
        if (pathString[i] == ';') {
            colonIndex = i;
        }
    }
    if (colonIndex == -1) { // if -1, a colon wasn't found in pathString
        err_msg(err);
    }
    path.size = get_path_size(pathString, colonIndex, err);   
    path.sites = get_path_sites(pathString + colonIndex + 1,
            path.size, numPlayers, err);
    place_initial_players(&path, numPlayers);
    free(pathString);
    return path;
}

/**
 * Extracts the size of the path from the path string
 * @param pathString the string format of path
 * @param colonIndex the index of the semicolon in the path string
 * @param err the type of com error to be thrown
 * @returns the size of the path (i.e. how many sites on path)
 */
int get_path_size(char* pathString, int colonIndex, Err err) {
    char sizeString[colonIndex + 1]; // +1 for null terminator
    strncpy(sizeString, pathString, colonIndex);
    sizeString[colonIndex] = '\0';
    int error;
    int size = string_to_int(sizeString, &error);
    if (error || size < 2) {
        err_msg(err);
    }
    return size;
}

/**
 * Extracts the sites from a pathstring
 * @param pathString the pathstring not including the size and ';' at start
 * @param size the number of sites in the path
 * @param numPlayers the number of players in the game
 * @param err the type of com error to be thrown
 * @returns "array" of sites with all the sites on the path
 */
Site* get_path_sites(char* pathString, int size, int numPlayers, Err err) {
    Site* sites = malloc(sizeof(Site) * size);
    char* siteBuffer = calloc(sizeof(char), CHARS_PER_SITE + 1); 
    for (int i = 0; i < size; i++) {
        strncpy(siteBuffer, pathString, CHARS_PER_SITE);
        siteBuffer[CHARS_PER_SITE] = '\0';
        pathString += CHARS_PER_SITE;
        sites[i] = string_to_site(siteBuffer, err, numPlayers);
        sites[i].siteNumber = i;
    }
    if (strlen(pathString) != 0) {
        err_msg(err); // there should not be any content after
    }
    // there should be barrier site at the start and end of path
    if (sites[0].type != BAR_SITE || sites[size - 1].type != BAR_SITE) {
        err_msg(err);
    }    
    free(siteBuffer);
    return sites;
}

/**
 * Gets the path from a file and returns it as a null terminated char*
 * @param stream the stream the path file is coming from
 * @returns null terminated char* representing path
 */
char* init_path_string(FILE* stream) {
    bool eof;
    char* pathString = get_line(stream, &eof);
    if (eof) {
        err_msg(BAD_DEALER_PATH);
    }
    strtok(pathString, "\n"); // remove trailing new line
    return pathString;
}

/**
 * Loads the contents of deck into deck struct from the specified stream
 * @param deckStream the stream with the deck file
 * @returns Initialised deck struct
 */
Deck init_deck(FILE* deckStream) {
    Deck deck;
    deck.count = 0; // 0 cards drawn initially
    deck.size = fgetc(deckStream) - '0';
    if (deck.size < 4) { // even if size was EOF, size - '0' would still be < 4
        err_msg(BAD_DECK);
    }
    // we add +2 to size because: + 1 for '\n'  deckfile and +1 for '\0'
    deck.values = malloc(sizeof(char) * (deck.size + 2)); 
    fgets(deck.values, deck.size + 2, deckStream);
    if (strlen(deck.values) != deck.size + 1) {
        err_msg(BAD_DECK);
    }
    if (deck.values[deck.size] != '\n') {
        err_msg(BAD_DECK);
    }
    if (fgetc(deckStream) != EOF) { 
        err_msg(BAD_DECK);
    } 
    for (int i = 0; i < deck.size; i++) {
        if (deck.values[i] < 'A' || deck.values[i] > 'E') {
            err_msg(BAD_DECK);
        }
    }
    return deck;
}

/**
 * Initialises all players in the game
 * @param numPlayers the number of players in this game
 * @returns pointer to array of all players in game
 */
Player* init_players(int numPlayers) {
    Player* players = (Player*) malloc(sizeof(Player) * numPlayers);
    for (int i = 0; i < numPlayers; i++) {
        players[i] = init_player(i);
    }
    return players;
}

/**
 * Initialises a single player with the default values for struct fields
 * @param id the id number for this player
 * @returns iniitalised Player struct
 */
Player init_player(int id) {
    Player player;
    player.playerId = id;
    player.site = 0; // players start at site 0
    player.cards = calloc(NUM_TYPE_CARD, sizeof(int));
    player.money = 7; // players start with 7 money
    player.points = 0;
    player.v1 = 0;
    player.v2 = 0;
    return player;
}

/**
 * Converts the char* representation of a site (e.g. V11 OR ::-) to decoded
 * site struct
 * @param site the char* representation of a site
 * @param err the error to be thrown in case of a bad path
 * (Because both player and dealer call this function.)
 * @param numPlayers the number of players in the game
 * @returns Site struct type rep of inputted string rep of a site 
 */
Site string_to_site(char* site, Err err, int numPlayers) {
    Site decodedSite; // struct form of site
    decodedSite.numPlayers = 0;
    decodedSite.capacity = (site[2] - '0');
    site[2] = '\0'; // only care about site chars now
    if (strcmp(site, "Mo") == 0) {
        decodedSite.type = MO_SITE;
    } else if (strcmp(site, "V1") == 0) {
        decodedSite.type = V1_SITE;
    } else if (strcmp(site, "V2") == 0) {
        decodedSite.type = V2_SITE;
    } else if (strcmp(site, "Do") == 0) {
        decodedSite.type = DO_SITE;
    } else if (strcmp(site, "Ri") == 0) {
        decodedSite.type = RI_SITE;
    } else if (strcmp(site, "::") == 0) {
        decodedSite.type = BAR_SITE;
    } else {
        err_msg(err);
    }
    if (decodedSite.type == BAR_SITE) {
        if (decodedSite.capacity != -3) {
            err_msg(err);
        }
        decodedSite.capacity = numPlayers;
    } else {
        if (decodedSite.capacity < 1 || decodedSite.capacity > 9) {
            err_msg(err);
        }
    }
    decodedSite.playerIds = calloc(decodedSite.capacity, sizeof(int));
    return decodedSite;       
}

/**
 * Places the player ids in the first site of the path in descending order
 * at the start of the game
 * @param path pointer to path struct for this game
 * @param numPlayers the number of players in this game
 */
void place_initial_players(Path* path, int numPlayers) {
    for (int i = numPlayers - 1; i >= 0; i--) {
        path->sites[0].playerIds[numPlayers - 1 - i] = i;
        path->sites[0].numPlayers++;
    }
}
