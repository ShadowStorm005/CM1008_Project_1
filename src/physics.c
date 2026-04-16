#include <stdio.h>
#include <SDL.h>
#include "physics.h"
#include "player.h"
#include "map.h"

void checkForCollisions(Player *pPlayer, Platform *pPlatforms, int platformCount, SDL_Renderer *pRenderer)
{
    SDL_Rect playerRect = getPlayerRect(pPlayer);
    SDL_Rect collisionResult;
    for (int i = 0; i < platformCount; i++){
        SDL_Rect platformRect = getPlatformRect(pPlatforms, i);
        if (SDL_IntersectRect(&playerRect, &platformRect, &collisionResult)){
            if (collisionResult.w > collisionResult.h){
                if (collisionResult.x==playerRect.x && collisionResult.y==playerRect.y){
                    // Player colliding from bottom
                    playerRect.y += collisionResult.h;
                    setPlayerRect(pPlayer, playerRect.x, playerRect.y);
                    updatePlayerRects(pPlayer);
                }
                else{
                    // Player colliding from top
                    playerRect.y -= collisionResult.h;
                    setPlayerGrounded(pPlayer);
                    setPlayerRect(pPlayer, playerRect.x, playerRect.y);
                    updatePlayerRects(pPlayer);
                }
            }
            else if (collisionResult.h > collisionResult.w){
                if (collisionResult.x==playerRect.x && collisionResult.y==playerRect.y){
                    // Player colliding from right
                    playerRect.x += collisionResult.w;
                    setPlayerRect(pPlayer, playerRect.x, playerRect.y);
                    updatePlayerRects(pPlayer);
                }
                else{
                    // Player colliding from left
                    playerRect.x -= collisionResult.w;
                    setPlayerRect(pPlayer, playerRect.x, playerRect.y);
                    updatePlayerRects(pPlayer);
                }
            }
        }
    }
}