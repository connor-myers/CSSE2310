/**
 * This file has all the game logic code in it
 */

#include "logic.h"

//private functions
void place_player(Site* site, Player* player);
void remove_player(Game* game, Player* player);
int find_next_barrier(Game* game, int startingSite);
bool is_valid_move(Game* game, Site newSite, Player player);

/**
 * Moves a player from the site they are currently on to a new site
 * @param game pointer to the main game struct
 * @param player pointer to the player to be moved
 * @param site pointer to the site the player should be moved to
 * @returns true if the player was sucessfully moved, false otherwise
 */
bool move_player(Game* game, Player* player, Site* newSite) {
    if (is_valid_move(game, *newSite, *player)) {
        remove_player(game, player);
        place_player(newSite, player);
        return true;
    }
    return false;
}

/**
 * Removes the player from whatever site they're on
 * @param game pointer to the main game struct
 * @param player pointer to the player to be moved
 * @returns true iff player was successfully removed from it's site
 */
void remove_player(Game* game, Player* player) {
    Site* site = &game->path.sites[player->site];
    int index = -1; // where the player is in the site
    for (int i = 0; i < site->numPlayers; i++) {
        if (site->playerIds[i] == player->playerId) {
            index = i;
        }
    }
    if (index != site->numPlayers - 1) {
        // move everyone in front of player backwards
        for (int i = index; i < site->numPlayers; i++) {
            site->playerIds[index] = site->playerIds[index + 1];
        }
    }
    site->numPlayers--;
}

/**
 * Attempts to place player at Site. Returns true iff placement is valid.
 * @param site the site for the player to be placed at
 * @param player the player to be placed
 * @returns true iff player was successfully placed at the new site
 */
void place_player(Site* site, Player* player) {
    site->playerIds[site->numPlayers] = player->playerId;
    player->site = site->siteNumber;
    site->numPlayers++;
}

/**
 * Checks to see if placing the player at a newSite would be a valid move.
 * I.e. they can't skip over a barrier and the newSite should have capacity
 * @param game pointer to main game struct
 * @param newSite the site to check if the player can move to
 * @param player the player that wants to move
 * @returns true iff player moving to newSite is valid
 */
bool is_valid_move(Game* game, Site newSite, Player player) {
    if (is_site_full(&newSite)) {
        return false;
    }
    if (newSite.siteNumber
            > find_next_site_type(game, BAR_SITE, player.site)) {
        return false;
    }
    return true;
}

/**
 * Returns true if the site is full, false if there is room
 * @param site pointer to site to check if it's full
 * @returns true if the site is full, false if there is room
 */
bool is_site_full(Site* site) {
    return site->numPlayers >= site->capacity;
}

/**
 * Finds the next site of type site type after the startingSite index
 * @param startingSite the index from which to start the search from
 * (not including that site)
 * @returns the index of the next site with Type, -1 if one can't be found
 */
int find_next_site_type(Game* game, SiteType type, int startingSite) {
    for (int i = startingSite + 1; i < game->path.size; i++) {
        if (game->path.sites[i].type == type) {
            return i;
        }        
    }
    return -1;
}

/**
 * Finds where a player is in the path
 * @param game pointer to main game struct
 * @param id the playerId for the player to be found
 * @param error the error to be thrown if player can't be found
 * @returns pointer to the player with specified id
 */
Player* find_player(Game* game, int id, Err error) {
    Player* p;
    PlayerCoord pc;
    pc.site = -1; // so returns invalid coord if player cant be found
    pc.siteIndex = -1;
    for (int i = 0; i < game->path.size; i++) {
        for (int j = 0; j < game->path.sites[i].capacity; j++) {
            if (game->path.sites[i].playerIds[j] == id) {
                pc.site = i;
                pc.siteIndex = j;
                break;
            }
        }
    }
    p = &game->players[game->path.sites[pc.site].playerIds[pc.siteIndex]];
    if (pc.site != -1 && pc.siteIndex != -1) {
        return p;
    }
    err_msg(error); // if player cant be found, an error has occured
    return p; // needed so compiler doesn't throw warning
}

/**
 * Updates Player after a HAP message
 * @param player pointer to the player to be updated 
 * @param newSite the site the player has moved to
 * @param message pointer to HAP message with values to update player with
 */
void update_player(Player* player, Site newSite, Message* message) {
    player->site = newSite.siteNumber;
    player->points += message->params[2];
    player->money += message->params[3];
    if (message->params[4] != '0') {
        // A is indexed at 0 in cards, since A is represented by '1'
        // we subtract - '0' to get 1 and then 1 to get 0
        // Similar applies for other cards
        player->cards[message->params[4] - '0' - 1]++; 
    }
    switch(newSite.type) {
        case V1_SITE:
            player->v1++;
            break;
        case V2_SITE:
            player->v2++;
            break;
        default:
            break;    
    }
}

/**
 * Returns the sum of all the cards this player has
 * @param player pointer to player to count the cards of 
 * @returns the number of cards the player has
 */
int sum_cards(Player* player) {
    int sum = 0;
    for (int i = 0; i < NUM_TYPE_CARD; i++) {
        sum += player->cards[i];
    }
    return sum;
}

/**
 * Gives all the players the points from their cards and sites visited
 * at the normal end of the game
 * @param game pointer to main game struct
 */
void distribute_extra_points(Game* game) {
    for (int i = 0; i < game->numPlayers; i++) {
        game->players[i].points
                += calculate_points_from_cards(&game->players[i]);
        game->players[i].points
                += calculate_points_from_sites(&game->players[i]);
    }
}