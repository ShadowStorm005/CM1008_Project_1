#ifndef FYSIKS_H
#define FYSIKS_H

#include <SDL.h>
#include "player.h"
#include "map.h"

void checkForCollisions(Player *pPlayer, Map *pMap);

#endif