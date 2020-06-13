#ifndef LOGIC_H
#define LOGIC_H

#include <stdbool.h>
#include <stdio.h>
#include "game.h"
#include "message.h"
#include "errs.h"
#include "utility.h"
#include "constants.h"

bool move_player(Game* game, Player* player, Site* newSite);
Player* find_player(Game* game, int id, Err error);
void update_player(Player* player, Site newSite, Message* message);
bool is_valid_move(Game* game, Site newSite, Player player);
bool is_site_full(Site* site);
int find_next_site_type(Game* game, SiteType type, int startingSite);
int sum_cards(Player* player);
void distribute_extra_points(Game* game);

#endif