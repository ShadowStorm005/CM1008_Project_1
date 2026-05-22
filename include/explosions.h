#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H

#include <SDL.h>

typedef struct explosion Explosion;

Explosion *createExplosion(SDL_Renderer *pRenderer);
int isExplosionActive(Explosion *pExplosion);
void activateExplosion(Explosion *pExplosion, int x, int y);
void drawExplosion(Explosion *pExplosion);
void destroyExplosion(Explosion *pExplosion);

#endif