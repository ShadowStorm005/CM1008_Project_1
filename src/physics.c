#include <stdio.h>
#include <SDL.h>
#include "physics.h"
#include "player.h"
#include "map.h"
#include "weapon.h"

void checkForPlayerCollision(Player *pPlayer, Map *pMap)
{
    SDL_Rect playerRect = getPlayerRect(pPlayer);
    SDL_Rect collisionResult;
    for (int i = 0; i < AMOUNT_OF_TILES_HORIZONTAL; i++){
        for (int j = 0; j < AMOUNT_OF_TILES_VERTICAL; j++){
            SDL_Rect tileRect = getTileRect(pMap, i, j);
            if (!isTileAktive(pMap, i, j)) continue;
            if (SDL_IntersectRect(&playerRect, &tileRect, &collisionResult)){
                if (collisionResult.w > collisionResult.h){
                    if (collisionResult.y > tileRect.y + (tileRect.h / 2)){
                        // Player colliding from bottom
                        playerRect.y += collisionResult.h;
                        stopVelY(pPlayer);
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
                    if (collisionResult.x > tileRect.x + (tileRect.w / 2)){
                        // Player colliding from right
                        playerRect.x += collisionResult.w;
                        touchingWall(pPlayer);
                        setPlayerRect(pPlayer, playerRect.x, playerRect.y);
                        updatePlayerRects(pPlayer);
                    }
                    else{
                        // Player colliding from left
                        playerRect.x -= collisionResult.w;
                        touchingWall(pPlayer);
                        setPlayerRect(pPlayer, playerRect.x, playerRect.y);
                        updatePlayerRects(pPlayer);
                    }
                }
            }
        }
    }
}

void checkForBulletCollision(Projectile *pProjectile, Map *pMap)
{
    SDL_Rect bulletRect = getBulletRect(pProjectile);
    for (int i = 0; i < AMOUNT_OF_TILES_HORIZONTAL; i++){
        for (int j = 0; j < AMOUNT_OF_TILES_VERTICAL; j++){
            SDL_Rect tileRect = getTileRect(pMap, i, j);
            if (!isTileAktive(pMap, i, j)) continue;
            if (SDL_HasIntersection(&bulletRect, &tileRect)){
                inactivateBullet(pProjectile);
                inactivateTile(pMap, i, j);
                triggerBulletExplosion(pMap, i, j);
                return;
            }
        }
    }
}

void triggerBulletExplosion(Map *pMap, int x, int y)
{
    int minX = x - 5;
    int maxX = x + 5;
    for (int i = minX; i <= maxX; i++){
        if (i <= x){
            for (int j = y - (i-minX); j <= y + (i-minX); j++){
                inactivateTile(pMap, i, j);
            }
        }
        else{
            for (int j = y - (maxX-i); j <= y + (maxX-i); j++){
                inactivateTile(pMap, i, j);
            }
        }
    }
}