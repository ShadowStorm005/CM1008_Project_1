#ifndef FYSIKS_H
#define FYSIKS_H

#include <SDL.h>
#include "player.h"
#include "map.h"
#include "weapon.h"
#include "explosions.h"
#include "game_net.h"

void checkForPlayerCollision(Player *pPlayer, Map *pMap);
void checkForBulletCollision(Projectile *pProjectile, Map *pMap, NetTile tileChanges[MAX_TILE_CHANGES], uint8_t *tileChangeCount);
void triggerBulletExplosion(Map *pMap, int x, int y, int radius, NetTile tileChanges[MAX_TILE_CHANGES], uint8_t *tileChangeCount);
void addChangedTile(NetTile tileChanges[MAX_TILE_CHANGES], uint8_t *tileChangeCount, int x, int y, int newTexture);
void checkBulletPlayerCollision(Projectile *pProjectile, Player *pPlayer);

#endif