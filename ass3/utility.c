#include "utility.h"

// private functions
bool cards_left(Player* player, int* cards);
char* site_to_string(Site site);
int get_num_levels(Game game);

/**
 * Safely converts a String (reprenting a non-negative number) to an int
 * @param string the string to be converted into an interger
 * @param error is changed to 1 if there was an error, 0 otherwise
 * @returns int version of string.
 */
int string_to_int(char* string, int* error) {
    char* ptr;
    int number = (int) strtol(string, &ptr, 10); // 10 for base-10
    *error = ((strlen(ptr) != 0) || (strlen(string) == 0));
    if (strlen(string) > 1 && string[0] == '0') {
        // checks for a leading 0.
        *error = 1;
    }
    return number;
}

/**
 * Converts an integer into a string
 * @param number the number to be converted
 * @returns char* string version of number
 */
char* int_to_string(int number) {
    // the + 1 in malloc is for the null terminator
    char* string = malloc(sizeof(char) * num_digits(number) + 1); 
    sprintf(string, "%d", number);
    return string;
}

/**
 * Finds the number of digits (base-10) in a number.
 * Used for memory allocation for int to string
 * @param number the number to find the number of digits for
 * @returns the number of base-10 digits in number;
 */
int num_digits(int number) {
    int digits = 0;
    do {
        digits++;
        number /= 10; // 10 for base-10
    } while (number != 0);
    return digits;
}

/**
* Gets a line from stream. A line is terminated at either '\n' or EOF.
* @param stream the stream to retrieve the line from
* @param eof sets this bool to true iff EOF was encountered when getting line
* @returns Returns line from stream (without the '\n' or EOF) as a null
* terminated char*.
*/
char* get_line(FILE* stream, bool* eof) {
    char* str = malloc(sizeof(char));
    int c;
    int i = 0;
    while((c = fgetc(stream)) != EOF) {
        str[i] = c;
        i++;
        str = realloc(str, sizeof(char) * (i + 1)); // incredibly inefficient
        if (c == '\n') {
            str[i] = '\0';
            break;
        }
    }
    str[i] = '\0';
    *eof = (c == EOF);
    return str;
}

/**
 * Determines whether the card is valid 
 * i.e. between A and E
 * @param card the char to be checked if it's a valid card
 * @returns true if the card is valid, false otherwise
 */
bool is_valid_card(char card) {
    return (card >= '0' && card <= '5');
}

/**
 * Calculates the score the player acquired from their deck of cards
 * @param player pointer to player struct
 * @returns points from cards for player
 */
int calculate_points_from_cards(Player* player) {
    int points = 0;
    while(cards_left(player, player->cards)) {
        int set = 0;
        for (int i = 0; i < NUM_TYPE_CARD; i++) {
            if (player->cards[i] > 0) {
                set++;
                player->cards[i]--;
            }        
        }
        switch(set) {
            case 5:
                points += 10;
                break;
            case 4:
                points += 7;
                break;
            case 3:
                points += 5;
                break;
            case 2:
                points += 3;
                break;
            case 1:
                points += 1;
                break;
            default:
                break;                   
        } 
    }
    return points;
}

/**
 * Calculates the extra points the player gets at the end of game having
 * visted V1 or V2 sites
 * @param player pointer to player struct
 * @returns points acquired from visited sites
 */
int calculate_points_from_sites(Player* player) {
    int points = 0;
    points += player->v1;
    points += player->v2;
    return points;
}

/**
 * Returns true if player stills has cards left
 * @param player pointer to player struct
 * @returns true if player has at least one card left
 */
bool cards_left(Player* player, int* cards) {
    for (int i = 0; i < NUM_TYPE_CARD; i++) {
        if (cards[i] > 0) {
            return true;
        }
    }    
    return false;
}


/**
 * Prints the player information to the specified stream
 * @param stream the stream to print the player info to
 * @param p pointer to player struct to print the information for
 */
void print_player_info(FILE* stream, Player* p) {
    fprintf(stream, "Player %d Money=%d V1=%d V2=%d "
            "Points=%d A=%d B=%d C=%d D=%d E=%d\n",
            p->playerId, p->money, p->v1, p->v2, p->points,
            p->cards[A], p->cards[B], p->cards[C], p->cards[D], p->cards[E]);
    fflush(stream);
}

/**
 * Prints the game path to specified stream
 * @param game the main game struct
 * @param stream the stream for the path to be printed to
 */
void print_path(Game game, FILE* stream) {
    for (int i = 0; i < game.path.size; i++) {
        fprintf(stream, "%s ", site_to_string(game.path.sites[i]));
    }
    for (int i = 0; i < get_num_levels(game); i++) {
        fprintf(stream, "\n"); // stop printing new lines when no players
        for (int j = 0; j < game.path.size; j++) {
            if (game.path.sites[j].numPlayers > i) {
                fprintf(stream, "%d  ", game.path.sites[j].playerIds[i]);
            } else {
                fprintf(stream, "   ");
            }
        }
    }
    fprintf(stream, "\n");
    fflush(stream);
    return;
}

/**
 * Prints player scores to indicated stream
 * @param game pointer to main game struct
 * @param stream the stream to be printed to
 */
void print_scores(Game* game, FILE* stream) {
    fprintf(stream, "Scores: ");
    for (int i = 0; i < game->numPlayers; i++) {
        fprintf(stream, "%d", game->players[i].points);
        if (i == game->numPlayers - 1) {
            fprintf(stream, "\n");
        } else {
            fprintf(stream, ",");
        }
    }
    fflush(stream);    
}

/**
 * Converts site type to the string that represents it
 * @param site the site struct
 * @returns string representation of the site
 */
char* site_to_string(Site site) {
    char* siteString = ""; // string version of site
    switch(site.type) {
        case MO_SITE:
            siteString = "Mo";
            break;
        case V1_SITE:
            siteString = "V1";
            break;    
        case V2_SITE:
            siteString = "V2";
            break;
        case DO_SITE:
            siteString = "Do";
            break;
        case RI_SITE:
            siteString = "Ri";
            break;
        case BAR_SITE:
            siteString = "::";
            break;                          
    }
    return siteString;
}

/**
 * Counts the number of levels that need to be printed when the path is printed
 * @param game the main game struct
 * @returns the number of levels that need to be printed
 */
int get_num_levels(Game game) {
    bool empty;
    for (int i = 0; i < game.numPlayers; i++) {
        empty = true;
        for (int j = 0; j < game.path.size; j++) {
            if (game.path.sites[j].numPlayers > i) {
                empty = false;
            }            
        }
        if (empty) {
            return i;
        }
    }
    return game.numPlayers;
}

/**
 * Checks if the hap message is valid before trying to act on it
 * @param message the message
 * @param game the game struct
 * @returns true iff the hap paramaters are valid
 */
bool is_valid_hap(Message message, Game game) {
    if (message.messageType != HAP) {
        return false;
    }
    if (message.numParams != NUM_HAP_PARAMS) {
        return false;
    }
    if (message.params[0] < 0 || message.params[0] >= game.numPlayers) {
        return false;
    }
    if (message.params[1] < 0
            || message.params[1] >= game.path.size) {
        return false;
    }
    if (message.params[2] < 0) {
        return false;
    }
    if (!is_valid_card(message.params[4])) {
        return false;
    }
    return true;
}

/**
 * Frees the dynamically allocated memory for a game
 * @param game main game struct
 */
void free_game(Game game) {
    for (int i = 0; i < game.numPlayers; i++) {
        free(game.players[i].cards);
    }
    free(game.players);    
    for (int i = 0; i < game.path.size; i++) {
        free(game.path.sites[i].playerIds);
    }
    free(game.path.sites);
}