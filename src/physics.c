#include <stdio.h>
#include <SDL.h>
#include <math.h>
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
            if (!isTileActive(pMap, i, j)) continue;
            if (SDL_IntersectRect(&playerRect, &tileRect, &collisionResult)){
                if (collisionResult.w > collisionResult.h){
                    if (collisionResult.y > tileRect.y + (tileRect.h / 2)){
                        // Player colliding from bottom
                        playerRect.y += collisionResult.h;
                        stopVelY(pPlayer);
                        setPlayerCord(pPlayer, playerRect.x, playerRect.y);
                        updatePlayerRects(pPlayer);
                    }
                    else{
                        // Player colliding from top
                        playerRect.y -= collisionResult.h;
                        setPlayerGrounded(pPlayer);
                        setPlayerCord(pPlayer, playerRect.x, playerRect.y);
                        updatePlayerRects(pPlayer);
                    }
                }
                else if (collisionResult.h > collisionResult.w){
                    if (collisionResult.x > tileRect.x + (tileRect.w / 2)){
                        // Player colliding from right
                        playerRect.x += collisionResult.w;
                        touchingWall(pPlayer);
                        setPlayerCord(pPlayer, playerRect.x, playerRect.y);
                        updatePlayerRects(pPlayer);
                    }
                    else{
                        // Player colliding from left
                        playerRect.x -= collisionResult.w;
                        touchingWall(pPlayer);
                        setPlayerCord(pPlayer, playerRect.x, playerRect.y);
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
            if (!isTileActive(pMap, i, j)) continue;
            if (SDL_HasIntersection(&bulletRect, &tileRect)){
                inactivateBullet(pProjectile);
                inactivateTile(pMap, i, j);
                triggerBulletExplosion(pMap, i, j, 4);
                return;
            }
        }
    }
}

void triggerBulletExplosion(Map *pMap, int x, int y, int radius)
{
    int minX = x - radius;
    int maxX = x + radius;
    for (int i = minX; i <= maxX; i++){
        if (i <= x){
            for (int j = y - (i-minX); j <= y + (i-minX); j++){
                if (!isTileActive(pMap, i, j)) continue;
                //setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j) + (int)hypotf(x-i, y-j));
                if (hypotf(x-i, y-j) < 1.2f) inactivateTile(pMap, i, j);
                else if (hypotf(x-i, y-j) < 2.2f) {
                    if ((getSelectedTexture(pMap, i, j)+5)%6 < (getSelectedTexture(pMap, i, j))%6)
                        inactivateTile(pMap, i, j);
                    else
                        setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j)+5);
                }
                else if (hypotf(x-i, y-j) < 3.2f) {
                    if ((getSelectedTexture(pMap, i, j)+3)%6 < (getSelectedTexture(pMap, i, j))%6)
                        inactivateTile(pMap, i, j);
                    else
                        setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j)+3);
                }
            }
        }
        else{
            for (int j = y - (maxX-i); j <= y + (maxX-i); j++){
                if (!isTileActive(pMap, i, j)) continue;
                //setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j) + (int)hypotf(i-x, y-j));
                if (hypotf(i-x, y-j) < 1.2f) inactivateTile(pMap, i, j);
                else if (hypotf(i-x, y-j) < 2.2f) {
                    if ((getSelectedTexture(pMap, i, j)+5)%6 < (getSelectedTexture(pMap, i, j))%6)
                        inactivateTile(pMap, i, j);
                    else
                        setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j)+5);
                }
                else if (hypotf(i-x, y-j) < 3.2f) {
                    if ((getSelectedTexture(pMap, i, j)+3)%6 < (getSelectedTexture(pMap, i, j))%6)
                        inactivateTile(pMap, i, j);
                    else
                        setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j)+3);
                }
            }
        }
    }
}