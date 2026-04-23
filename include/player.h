#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include "map.h"
#include "weapon.h"

typedef struct player Player;

void updatePlayerRects(Player *pPlayer);
Player *createPlayer(float x, float y, SDL_Renderer *pRenderer, int window_width, int window_height);
float getXCord(Player *pPlayer);
float getYCord(Player *pPlayer);
float getAngle(Player *pPlayer);
int canShoot(Player *pPlayer);
void enableTrigger(Player *pPlayer, int enable);
void moveLeft(Player *pPlayer);
void moveRight(Player *pPlayer);
void jump(Player *pPlayer);
void setCanonMode(Player *pPlayer, int mode);
int getCanonMode(Player *pPlayer);
void deaccelerate(Player *pPlayer);
void updatePlayer(Player *pPlayer, Map *tiles);
void drawPlayer(Player *pPlayer);
SDL_Rect getPlayerHitbox(Player *pPlayer);
SDL_Rect getPlayerRect(Player *pPlayer);
void setPlayerCord(Player *pPlayer, int x, int y);
void setPlayerGrounded(Player *pPlayer);
void stopVelY(Player *pPlayer);
void stopVelX(Player *pPlayer);
void touchingWall(Player *pPlayer);
void destroyPlayer(Player *pPlayer);

#endif