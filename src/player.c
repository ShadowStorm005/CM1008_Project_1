#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "player.h"
#include "weapon.h"
#include "map.h"
#include "physics.h"

#define MAX_VELY_SPEED 15.0f

struct player {
    int health;

    float x, y;
    float velX, velY;

    float moveSpeed;
    float jumpForce;
    float gravity;
    int canFire;
    int isGrounded;
    int isTouchingWall;

    int window_width, window_height;

    float canonAngle;

    SDL_Texture *pTexture;
    SDL_Renderer *pRenderer;

    SDL_Texture *pCanonTx;
    SDL_Renderer *pCanonRn;

    SDL_Rect playerRect;
    SDL_Rect hitbox;
    SDL_Rect canonRect;

    SDL_RendererFlip flip;
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
    pPlayer->canFire = 1;

    pPlayer->velX = 0.0f;
    pPlayer->velY = 0.0f;
    pPlayer->moveSpeed = 2.0f;
    pPlayer->jumpForce = 12.0f;
    pPlayer->gravity = 0.7f;
    pPlayer->isGrounded = 0;
    pPlayer->isTouchingWall = 0;

    SDL_Surface *pSurface = IMG_Load("Resources/firsttank.png");
    if (!pSurface) 
    {
        printf("Error loading firsttank.png: %s\n", IMG_GetError());
        free(pPlayer);
        return NULL;
    }

    pPlayer->pRenderer = pRenderer;
    pPlayer->pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    pSurface = IMG_Load("Resources/primitivplatform.png");
    if (!pSurface)
    {
        printf("Error loading primitivplatform.png: %s\n", IMG_GetError());
        free(pPlayer);
        return NULL;
    }

    pPlayer->pCanonTx = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    free(pSurface);

    if (!pPlayer->pTexture) 
    {
        printf("Error creating player texture: %s\n", SDL_GetError());
        free(pPlayer);
        return NULL;
    }
    
    if (!pPlayer->pCanonTx) 
    {
        printf("Error creating canon texture: %s\n", SDL_GetError());
        free(pPlayer);
        return NULL;
    }

    SDL_QueryTexture(pPlayer->pTexture, NULL, NULL, &pPlayer->playerRect.w, &pPlayer->playerRect.h);
    SDL_QueryTexture(pPlayer->pCanonTx, NULL, NULL, &pPlayer->canonRect.w, &pPlayer->canonRect.h);

    pPlayer->playerRect.w = 32*1.4f;
    pPlayer->playerRect.h = 31*1.4f;

    pPlayer->canonRect.w = 65;
    pPlayer->canonRect.h = 30;

    pPlayer->x = x - pPlayer->playerRect.w / 2.0f;
    pPlayer->y = y - pPlayer->playerRect.h / 2.0f;

    pPlayer->flip = SDL_FLIP_NONE;

    updatePlayerRects(pPlayer);
    return pPlayer;
}

void moveLeft(Player *pPlayer)
{
    pPlayer->velX += -pPlayer->moveSpeed;
    pPlayer->flip = SDL_FLIP_HORIZONTAL;
}

void moveRight(Player *pPlayer)
{
    pPlayer->velX += pPlayer->moveSpeed;
    pPlayer->flip = SDL_FLIP_NONE;
}

void jump(Player *pPlayer)
{   
    if(pPlayer->isGrounded)
    {
        pPlayer->velY = -pPlayer->jumpForce;
        pPlayer->isGrounded = 0;
    }
}

float getXCord(Player *pPlayer)
{
    return pPlayer->x + (pPlayer->playerRect.w) / 2;
}

float getYCord(Player *pPlayer)
{
    return pPlayer->y;
}

void deaccelerate(Player *pPlayer)
{
    pPlayer->velX *= 0.8f;
    if(fabs(pPlayer->velX) < 0.1f) pPlayer->velX = 0;
}

int canShoot(Player *pPlayer)
{
    return pPlayer->canFire;
}

void enableTrigger(Player *pPlayer, int enable)
{
    if(enable)
    {
        pPlayer->canFire = 1;
    }
    else
    {
        pPlayer->canFire = 0;
    }
}

void updatePlayer(Player *pPlayer, Map *pMap)
{
    int mousePosx, mousePosy;
    Uint32 buttons = SDL_GetMouseState(&mousePosx, &mousePosy);
    SDL_Rect previousHitbox = pPlayer->hitbox;

    deaccelerate(pPlayer);

    pPlayer->velY += pPlayer->gravity;
    if (pPlayer->velY > MAX_VELY_SPEED) pPlayer->velY = MAX_VELY_SPEED;
    else if (pPlayer->velY < -MAX_VELY_SPEED) pPlayer->velY = -MAX_VELY_SPEED;

    pPlayer->x += pPlayer->velX;
    pPlayer->y += pPlayer->velY;

    updatePlayerRects(pPlayer);
    pPlayer->isGrounded = 0;
    
    checkForPlayerCollision(pPlayer, pMap);

    float dx = mousePosx - pPlayer->x;
    float dy = mousePosy - pPlayer->y;

    pPlayer->canonAngle = atan2(dy, dx);

    if (pPlayer->x < 0) pPlayer->x = 0;
    if (pPlayer->x + pPlayer->playerRect.w > pPlayer->window_width)
        pPlayer->x = pPlayer->window_width - pPlayer->playerRect.w;

    if (pPlayer->y + pPlayer->playerRect.h > pPlayer->window_height) 
    {
        pPlayer->y = pPlayer->window_height - pPlayer->playerRect.h;
        pPlayer->velY = 0.0f;
        pPlayer->isGrounded = 1;
    }

    if (pPlayer->y < 0) 
    {
        pPlayer->y = 0;
        pPlayer->velY = 0.0f;
    }

    updatePlayerRects(pPlayer);
}

void drawPlayer(Player *pPlayer)
{
    SDL_Point canonCenter = {0, pPlayer->canonRect.h / 2};

    SDL_RenderCopyEx(pPlayer->pRenderer, pPlayer->pTexture, NULL, &pPlayer->playerRect, 0.0, NULL, pPlayer->flip);
    SDL_RenderCopyEx(pPlayer->pRenderer, pPlayer->pCanonTx, NULL, &pPlayer->playerRect, pPlayer->canonAngle*180/3.141f, &canonCenter, pPlayer->flip);
    
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

void setPlayerCord(Player *pPlayer, int x, int y)
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

void stopVelX(Player *pPlayer)
{
    pPlayer->velX = 0.0f;
}

void touchingWall(Player *pPlayer)
{
    if (pPlayer->velY < 0) pPlayer->velY /= 1.5f;
    pPlayer->isTouchingWall = 1;
    pPlayer->velX = 0.0f;
}

void destroyPlayer(Player *pPlayer)
{
    if (!pPlayer) return;
    if (pPlayer->pTexture) SDL_DestroyTexture(pPlayer->pTexture);
    free(pPlayer);
}