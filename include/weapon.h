#ifndef WEAPON_H
#define WEAPON_H
#define MAX_BULLETS 200
#define PI 3.141592653589f
#define PROJECTILE_GRAVITY 0.15f
#define BULLET_SIZE 20

#include <SDL.h>
#include "game_net.h"

typedef struct projectile Projectile;

Projectile *createProjectile(SDL_Renderer *pRenderer);
int isActive(Projectile *pProjectile);
void updateProjectileRect(Projectile *pProjectile);
void updateProjectile(Projectile *pProjectile, Map *pMap, NetTile tileChanges[MAX_TILE_CHANGES], uint8_t *tileChangeCount);
void setProjectileVar(Projectile *pProjectile, int active, float x, float y, float angle);
void drawProjectile(Projectile *pProjectile);
void shoot(Projectile *pProjectile[], int size, float speed, float x, float y, float angle);
void inactivateBullet(Projectile *pProjectile);
SDL_Rect getBulletRect(Projectile *pProjectile);
void destroyProjectile(Projectile *pProjectile);
float getBulletX(Projectile *pProjectile);
float getBulletY(Projectile *pProjectile);
float getBulletAngle(Projectile *pProjectile);

#endif