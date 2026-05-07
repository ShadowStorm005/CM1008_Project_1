#include <stdio.h>
#include <SDL.h>
#include <math.h>
#include "physics.h"
#include "player.h"
#include "map.h"
#include "weapon.h"
#include "server_net.h"

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

void checkForBulletCollision(Projectile *pProjectile, Map *pMap, NetTile tileChanges[MAX_TILE_CHANGES], uint8_t tileChangeCount)
{
    SDL_Rect bulletRect = getBulletRect(pProjectile);
    for (int i = 0; i < AMOUNT_OF_TILES_HORIZONTAL; i++){
        for (int j = 0; j < AMOUNT_OF_TILES_VERTICAL; j++){
            SDL_Rect tileRect = getTileRect(pMap, i, j);
            if (!isTileActive(pMap, i, j)) continue;
            if (SDL_HasIntersection(&bulletRect, &tileRect)){
                inactivateBullet(pProjectile);
                inactivateTile(pMap, i, j);
                addChangedTile(tileChanges, &tileChangeCount, i, j, -1);
                triggerBulletExplosion(pMap, i, j, 4, tileChanges, &tileChangeCount);
                return;
            }
        }
    }
}

void triggerBulletExplosion(Map *pMap, int x, int y, int radius, NetTile tileChanges[MAX_TILE_CHANGES], uint8_t tileChangeCount)
{
    int minX = x - radius;
    int maxX = x + radius;
    for (int i = minX; i <= maxX; i++){
        if (i <= x){
            for (int j = y - (i-minX); j <= y + (i-minX); j++){
                if (!isTileActive(pMap, i, j)) continue;
                if (hypotf(x-i, y-j) < 1.2f) {
                    inactivateTile(pMap, i, j);
                    addChangedTile(tileChanges, &tileChangeCount, i, j, -1);
                }
                else if (hypotf(x-i, y-j) < 2.4f) {
                    if ((getSelectedTexture(pMap, i, j)+2)%3 <= (getSelectedTexture(pMap, i, j))%3) {
                        inactivateTile(pMap, i, j);
                        addChangedTile(tileChanges, &tileChangeCount, i, j, -1);
                    }
                    else {
                        setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j)+2);
                        addChangedTile(tileChanges, &tileChangeCount, i, j, getSelectedTexture(pMap, i, j));
                    }
                }
                else if (hypotf(x-i, y-j) < 3.2f) {
                    if ((getSelectedTexture(pMap, i, j)+1)%3 <= (getSelectedTexture(pMap, i, j))%3) {
                        inactivateTile(pMap, i, j);
                        addChangedTile(tileChanges, &tileChangeCount, i, j, -1);
                    }
                    else {
                        setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j)+1);
                        addChangedTile(tileChanges, &tileChangeCount, i, j, getSelectedTexture(pMap, i, j));
                    }
                }
            }
        }
        else{
            for (int j = y - (maxX-i); j <= y + (maxX-i); j++){
                if (!isTileActive(pMap, i, j)) continue;
                if (hypotf(i-x, y-j) < 1.2f) {
                    inactivateTile(pMap, i, j);
                    addChangedTile(tileChanges, &tileChangeCount, i, j, -1);
                }
                else if (hypotf(i-x, y-j) < 2.4f) {
                    if ((getSelectedTexture(pMap, i, j)+2)%3 <= (getSelectedTexture(pMap, i, j))%3) {
                        inactivateTile(pMap, i, j);
                        addChangedTile(tileChanges, &tileChangeCount, i, j, -1);
                    }
                    else {
                        setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j)+2);
                        addChangedTile(tileChanges, &tileChangeCount, i, j, getSelectedTexture(pMap, i, j));
                    }
                }
                else if (hypotf(i-x, y-j) < 3.2f) {
                    if ((getSelectedTexture(pMap, i, j)+1)%3 <= (getSelectedTexture(pMap, i, j))%3) {
                        inactivateTile(pMap, i, j);
                        addChangedTile(tileChanges, &tileChangeCount, i, j, -1);
                    }
                    else {
                        setSelectedTexture(pMap, i, j, getSelectedTexture(pMap, i, j)+1);
                        addChangedTile(tileChanges, &tileChangeCount, i, j, getSelectedTexture(pMap, i, j));
                    }
                }
            }
        }
    }
}