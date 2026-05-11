#ifndef FYSIKS_H
#define FYSIKS_H

#include <SDL.h>
#include "player.h"
#include "map.h"
#include "weapon.h"
#include "sound.h"

typedef struct sounds Sounds;

typedef struct sounds Sounds;

void checkForPlayerCollision(Player *pPlayer, Map *pMap);
void checkForBulletCollision(Projectile *pProjectile, Map *pMap, Sounds *sounds);
void triggerBulletExplosion(Map *pMap, int x, int y, int radius);

#endif