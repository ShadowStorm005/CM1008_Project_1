#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "player.h"
#include "map.h"
#include "physics.h"

struct player {
    int health;

    float x, y;
    float velX, velY;

    float moveSpeed;
    float jumpForce;
    float gravity;

    int isGrounded;
    int isTouchingWall;

    int window_width, window_height;

    SDL_Texture *pTexture;
    SDL_Renderer *pRenderer;

    SDL_Rect playerRect;
    SDL_Rect hitbox;
};

void updatePlayerRects(Player *pPlayer)
{
    pPlayer->playerRect.x = (int)pPlayer->x;
    pPlayer->playerRect.y = (int)pPlayer->y;

    pPlayer->hitbox.x = pPlayer->playerRect.x + 12;
    pPlayer->hitbox.y = pPlayer->playerRect.y + 14;
    pPlayer->hitbox.w = pPlayer->playerRect.w - 24;
    pPlayer->hitbox.h = pPlayer->playerRect.h - 18;
}

Player *createPlayer(float x, float y, SDL_Renderer *pRenderer, int window_width, int window_height)
{
    Player *pPlayer = malloc(sizeof(struct player));
    if (!pPlayer) return NULL;

    pPlayer->window_width = window_width;
    pPlayer->window_height = window_height;
    pPlayer->health = 100;

    pPlayer->velX = 0.0f;
    pPlayer->velY = 0.0f;
    pPlayer->moveSpeed = 1.2f;
    pPlayer->jumpForce = 14.0f;
    pPlayer->gravity = 0.7f;
    pPlayer->isGrounded = 0;
    pPlayer->isTouchingWall = 0;

    SDL_Surface *pSurface = IMG_Load("Resources/tank.png");
    if (!pSurface) {
        printf("Error loading tank.png: %s\n", IMG_GetError());
        free(pPlayer);
        return NULL;
    }

    pPlayer->pRenderer = pRenderer;
    pPlayer->pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);

    if (!pPlayer->pTexture) {
        printf("Error creating player texture: %s\n", SDL_GetError());
        free(pPlayer);
        return NULL;
    }

    SDL_QueryTexture(pPlayer->pTexture, NULL, NULL, &pPlayer->playerRect.w, &pPlayer->playerRect.h);

    pPlayer->playerRect.w = 110;
    pPlayer->playerRect.h = 60;

    pPlayer->x = x - pPlayer->playerRect.w / 2.0f;
    pPlayer->y = y - pPlayer->playerRect.h / 2.0f;

    updatePlayerRects(pPlayer);
    return pPlayer;
}

void moveLeft(Player *pPlayer)
{
    pPlayer->velX += -pPlayer->moveSpeed;
}

void moveRight(Player *pPlayer)
{
    pPlayer->velX += pPlayer->moveSpeed;
}

void jump(Player *pPlayer)
{   
    if(pPlayer->isGrounded)
    {
        pPlayer->velY = -pPlayer->jumpForce;
        pPlayer->isGrounded = 0;
    }
}

void deaccelerate(Player *pPlayer)
{
    pPlayer->velX *= 0.8f;
    if(fabs(pPlayer->velX) < 0.1f) pPlayer->velX = 0;
}

void updatePlayer(Player *pPlayer, Platform *platforms, int platformCount)
{
    SDL_Rect previousHitbox = pPlayer->hitbox;

    deaccelerate(pPlayer);

    pPlayer->velY += pPlayer->gravity;

    pPlayer->x += pPlayer->velX;
    pPlayer->y += pPlayer->velY;

    updatePlayerRects(pPlayer);
    pPlayer->isGrounded = 0;
    
    checkForCollisions(pPlayer, platforms, platformCount);

    /*for (int i = 0; i < platformCount; i++) {
        SDL_Rect platformRect = getPlatformRect(platforms, i);

        if (SDL_HasIntersection(&pPlayer->hitbox, &platformRect)) {
            int overlapLeft = (pPlayer->hitbox.x + pPlayer->hitbox.w) - platformRect.x;
            int overlapRight = (platformRect.x + platformRect.w) - pPlayer->hitbox.x;
            int overlapTop = (pPlayer->hitbox.y + pPlayer->hitbox.h) - platformRect.y;
            int overlapBottom = (platformRect.y + platformRect.h) - pPlayer->hitbox.y;

            int minOverlapX = overlapLeft < overlapRight ? overlapLeft : overlapRight;
            int minOverlapY = overlapTop < overlapBottom ? overlapTop : overlapBottom;

            if (minOverlapY < minOverlapX) {
                if (previousHitbox.y + previousHitbox.h <= platformRect.y) {
                    pPlayer->y = platformRect.y - pPlayer->playerRect.h + 4;
                    pPlayer->velY = 0.0f;
                    pPlayer->isGrounded = 1;
                    updatePlayerRects(pPlayer);
                }
                else if (previousHitbox.y >= platformRect.y + platformRect.h) {
                    pPlayer->y = platformRect.y + platformRect.h - 14;
                    pPlayer->velY = 0.0f;
                    updatePlayerRects(pPlayer);
                }
            } else {
                if (previousHitbox.x + previousHitbox.w <= platformRect.x) {
                    pPlayer->x -= minOverlapX;
                    updatePlayerRects(pPlayer);
                }
                else if (previousHitbox.x >= platformRect.x + platformRect.w) {
                    pPlayer->x += minOverlapX;
                    updatePlayerRects(pPlayer);
                }
            }
        }
    }*/

    if (pPlayer->x < 0) pPlayer->x = 0;
    if (pPlayer->x + pPlayer->playerRect.w > pPlayer->window_width)
        pPlayer->x = pPlayer->window_width - pPlayer->playerRect.w;

    if (pPlayer->y + pPlayer->playerRect.h > pPlayer->window_height) {
        pPlayer->y = pPlayer->window_height - pPlayer->playerRect.h;
        pPlayer->velY = 0.0f;
        pPlayer->isGrounded = 1;
    }

    if (pPlayer->y < 0) {
        pPlayer->y = 0;
        pPlayer->velY = 0.0f;
    }

    updatePlayerRects(pPlayer);
}

void drawPlayer(Player *pPlayer)
{
    SDL_RenderCopy(pPlayer->pRenderer, pPlayer->pTexture, NULL, &pPlayer->playerRect);

    SDL_SetRenderDrawColor(pPlayer->pRenderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(pPlayer->pRenderer, &pPlayer->hitbox);
}

SDL_Rect getPlayerHitbox(Player *pPlayer)
{
    return pPlayer->hitbox;
}

SDL_Rect getPlayerRect(Player *pPlayer)
{
    return pPlayer->playerRect;
}

void setPlayerRect(Player *pPlayer, int x, int y)
{
    pPlayer->x = x;
    pPlayer->y = y;
}

void setPlayerGrounded(Player *pPlayer)
{
    pPlayer->velY = 0.0f;
    pPlayer->isGrounded = 1;
    pPlayer->isTouchingWall = 0;
}

void stopVelY(Player *pPlayer)
{
    pPlayer->velY = 0.0f;
}

void touchingWall(Player *pPlayer)
{
    if (pPlayer->velY < 0) pPlayer->velY /= 1.5f;
    pPlayer->isTouchingWall = 1;
}

void destroyPlayer(Player *pPlayer)
{
    if (!pPlayer) return;
    if (pPlayer->pTexture) SDL_DestroyTexture(pPlayer->pTexture);
    free(pPlayer);
}

