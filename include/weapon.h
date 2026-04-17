#ifndef WEAPON_H
#define WEAPON_H
#define MAX_BULLETS 200

#include <SDL.h>

typedef struct projectile Projectile;

Projectile *createProjectile(SDL_Renderer *pRenderer);
int isActive(Projectile *pProjectile);
void shoot(Projectile *pProjectile[], float x, float y);
void updateProjectile(Projectile *pProjectile);
void drawProjectile(Projectile *pProjectile);
void destroyProjectile(Projectile *pProjectile);

#endif