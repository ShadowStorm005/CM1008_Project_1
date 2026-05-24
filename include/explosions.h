#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H

#include <SDL.h>

typedef struct explosion Explosion;

Explosion *createExplosion(SDL_Renderer *pRenderer);
void updateExplosionTexture(Explosion *pExplosion);
int getExplosionCordX(Explosion *pExplosion);
int getExplosionCordY(Explosion *pExplosion);
int isExplosionActive(Explosion *pExplosion);
int getStartTime(Explosion *pExplosion);
void receiveExplosionServerTime(Explosion *pExplosion, int serverTime);
void activateExplosion(Explosion *pExplosion, int x, int y, int startTime);
void drawExplosion(Explosion *pExplosion);
void destroyExplosion(Explosion *pExplosion);

#endif