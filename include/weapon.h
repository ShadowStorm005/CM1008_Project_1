#ifndef WEAPON_H
#define WEAPON_H
#define MAX_BULLETS 200
#define PI 3.141592653589f

#include <SDL.h>

typedef struct projectile Projectile;

Projectile *createProjectile(SDL_Renderer *pRenderer);
int isActive(Projectile *pProjectile);
void shoot(Projectile *pProjectile[], int size, float speed, float x, float y, float angle);
void updateProjectile(Projectile *pProjectile);
void drawProjectile(Projectile *pProjectile);
void destroyProjectile(Projectile *pProjectile);

#endif