#ifndef SERVER_CREATION_FUNCTIONS_H
#define SERVER_CREATION_FUNCTIONS_H

#include "player.h"
#include "weapon.h"
#include "map.h"
#include "explosions.h"

Player *createServerPlayer(float x, float y, int window_width, int window_height);

Projectile *createServerProjectile(void);
Explosion *createServerExplosion(void);

Map *createServerMap(int window_width, int window_height);

#endif