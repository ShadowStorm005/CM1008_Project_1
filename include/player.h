#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include "map.h"
#include "weapon.h"

typedef struct player Player;

Player *createPlayer(float x, float y, SDL_Renderer *pRenderer, int window_width, int window_height);
float getXCord(Player *pPlayer);
float getYCord(Player *pPlayer);
int canShoot(Player *pPlayer);
void enableTrigger(Player *pPlayer, int enable);
void jump(Player *pPlayer);
void moveLeft(Player *pPlayer);
void moveRight(Player *pPlayer);
void deaccelerate(Player *pPlayer);
void updatePlayer(Player *pPlayer, Platform *platforms, int platformCount);
void drawPlayer(Player *pPlayer);
SDL_Rect getPlayerHitbox(Player *pPlayer);
void destroyPlayer(Player *pPlayer);

#endif