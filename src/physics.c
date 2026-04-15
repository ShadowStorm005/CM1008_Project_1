#include <stdio.h>
#include <SDL.h>
#include "physics.h"
#include "player.h"
#include "map.h"

void checkForCollisions(Player *pPlayer, Platform *pPlatforms, int platformCount, SDL_Renderer *pRenderer)
{
    SDL_Rect playerHitbox = getPlayerHitbox(pPlayer);
    SDL_Rect collisionResult;
    for (int i=0; i<platformCount; i++){
        SDL_Rect platformRect = getPlatformRect(pPlatforms, platformCount);
        if (SDL_IntersectRect(&playerHitbox, &platformRect, &collisionResult) == SDL_TRUE){
            SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
            SDL_RenderDrawRect(pRenderer, &collisionResult);
        }
    }
}