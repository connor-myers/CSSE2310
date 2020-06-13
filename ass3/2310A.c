#include "players.h"

// private functions
int move_logic(Game* game, Player* player);

int main(int argc, char** argv) {
    run(argc, argv, &move_logic);
}

/**
 * Determines how this player will handle a 'YT' message
 * @param game pointer to the main game struct
 * @param player pointer to this player
 * @returns the site number the player wants to move to or returns -1 if
 * there was no valid site to move to (shouldn't happen)
 */
int move_logic(Game* game, Player* player) {
    // The player has money, and there is a Do site in front of them, go 
    if (player->money > 0) {
        for (int i = player->site + 1; i < game->path.size; i++) {
            if (game->path.sites[i].type == DO_SITE) {
                if (!is_valid_move(game, game->path.sites[i], *player)) {
                    continue;
                }
                return i;
            }
        }
    }
    // If the next site is Mo, then go there
    if (game->path.sites[player->site + 1].type == MO_SITE) {
        if (is_valid_move(game, game->path.sites[player->site + 1],
                *player)) {
            return player->site + 1;        
        }
    }
    // Pick the closest V1, V2 or ::, then go there
    for (int i = player->site + 1; i < game->path.size; i++) {
        SiteType type = game->path.sites[i].type;
        if (type == V1_SITE || type == V2_SITE || type == BAR_SITE) {
            if (!is_valid_move(game, game->path.sites[i], *player)) {
                continue;
            }
            return i;
        }
    } 
    return -1; // will never happen; -1 returned so it's an invalid site
}