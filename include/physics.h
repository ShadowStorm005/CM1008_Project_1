#ifndef FYSIKS_H
#define FYSIKS_H

#include <SDL.h>
#include "player.h"
#include "map.h"
#include "weapon.h"

void checkForPlayerCollision(Player *pPlayer, Map *pMap);
void checkForBulletCollision(Projectile *pProjectile, Map *pMap);

#endif