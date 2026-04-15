#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include "map.h"

typedef struct player Player;

Player *createPlayer(float x, float y, SDL_Renderer *pRenderer, int window_width, int window_height);
void updatePlayer(Player *pPlayer, const Uint8 *keystate, Platform *platforms, int platformCount);
void drawPlayer(Player *pPlayer);
SDL_Rect getPlayerHitbox(Player *pPlayer);
SDL_Rect getPlayerRect(Player *pPlayer);
void destroyPlayer(Player *pPlayer);

#endif