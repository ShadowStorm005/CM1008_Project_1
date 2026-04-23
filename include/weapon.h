#ifndef WEAPON_H
#define WEAPON_H
#define MAX_BULLETS 200

#include <SDL.h>

typedef struct projectile Projectile;

Projectile *createProjectile(SDL_Renderer *pRenderer);
int isActive(Projectile *pProjectile);
void updateProjectileRect(Projectile *pProjectile);
void updateProjectile(Projectile *pProjectile, Map *pMap);
void drawProjectile(Projectile *pProjectile);
void shoot(Projectile *pProjectile[], int size, float speed, float x, float y, float angle);
void inactivateBullet(Projectile *pProjectile);
SDL_Rect getBulletRect(Projectile *pProjectile);
void destroyProjectile(Projectile *pProjectile);

#endif