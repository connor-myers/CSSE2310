#include "players.h"

// private functions
int move_logic(Game* game, Player* player);
bool compare_cards(Game* game, Player* player);

int main(int argc, char** argv) {
    run(argc, argv, &move_logic);
}

/**
 * Determines how this player will handle a 'YT' message
 * @param game pointer to the main game struct
 * @param player pointer to this player
 * @returns the site number the player wants to move to
 * or -1 if there was no valid site to move to (shouldn't happen)
 */
int move_logic(Game* game, Player* player) {
    // next site capacity && all players on later sites -> move down 1 site
    if (!is_site_full(&game->path.sites[player->site + 1])) {
        int ourSite = player->site;
        bool validMove = true;
        for (int i = 0; i < game->numPlayers; i++) {
            if (i == player->playerId) {
                continue; // don't want to include our selves in this check
            }
            if (ourSite >= game->players[i].site) {
                validMove = false; // if we find a single player behind us
                break;
            }
        }
        if (validMove) {
            return player->site + 1; // + 1 for next site
        }        
    }
    int nextBar = find_next_site_type(game, BAR_SITE, player->site);
    // If odd amount of money, and Mo between next BAR, then go there.
    if (player->money % 2 == 1) { // checking for odd amount of money
        for (int i = player->site; i < game->path.size - 1; i++) {
            int nextMo = find_next_site_type(game, MO_SITE, i);
            if ((nextMo < nextBar) && (nextMo > 0)
                    && !is_site_full(&game->path.sites[nextMo])) {
                return nextMo;
            }            
        }
    }
    // go to next Ri if Ri before BAR & we have most cards | all have 0 cards
    if (compare_cards(game, player)) {
        int nextRi = find_next_site_type(game, RI_SITE, player->site);
        if ((nextRi < nextBar) && (nextRi > 0)
                && !is_site_full(&game->path.sites[nextRi])) {
            return nextRi;
        }
    }
    // If there is a V2 between us and the next barrier, then go there.
    int nextV2 = find_next_site_type(game, V2_SITE, player->site);
    if ((nextV2 < nextBar) && (nextV2 > 0)
            && !is_site_full(&game->path.sites[nextV2])) {
        return nextV2;
    }
    // Move forward to the earliest site which has room.
    for (int i = player->site + 1; i < game->path.size; i++) {
        if (!is_site_full(&game->path.sites[i])) {
            return i;
        }
    }
    return -1; // will never happen; -1 returned so it's an invalid site
}

/**
 * Checks to see if the specified player has the most cards or if everyone
 * has 0 cards.
 * @param game pointer to main game struct
 * @param player pointer to the player to check
 * @returns true iff the specified player has the most cards OR if all
 * players have 0 cards.
 */
bool compare_cards(Game* game, Player* player) {
    int ourSum = sum_cards(player);
    int theirSum = 0;
    for (int i = 0; i < game->numPlayers; i++) {
        if (i == player->playerId) {
            continue; // don't want to compare to ourselves
        }
        theirSum = sum_cards(&game->players[i]);
        if (ourSum == 0 && theirSum > 0) {
            return false;
        }
        if ((theirSum >= ourSum) && (theirSum != 0)) {
            return false;
        }
    }
    return true;
}