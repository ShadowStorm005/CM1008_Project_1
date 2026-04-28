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
    int canonMode;

    int window_width, window_height;

    float canonAngle;
    float targetAngle;

    SDL_Texture *pHullTx;
    SDL_Texture *pCanonTx;
    SDL_Texture *pTurretTx;

    SDL_Renderer *pRenderer;

    SDL_Rect hullRect;
    SDL_Rect hitbox;
    SDL_Rect canonRect;
    SDL_Rect turretRect;

    SDL_RendererFlip tankFlip;
    SDL_RendererFlip turretFlip;
};

void updatePlayerRects(Player *pPlayer)
{
    pPlayer->hullRect.x = (int)pPlayer->x;
    pPlayer->hullRect.y = (int)pPlayer->y;

    pPlayer->turretRect.y = (int)pPlayer->y - 7*(pPlayer->hullRect.h)/6;
    pPlayer->canonRect.y = (int)pPlayer->y - 3*(pPlayer->hullRect.h)/5;

    if(pPlayer->turretFlip == SDL_FLIP_NONE)
    {
        pPlayer->turretRect.x = (int)pPlayer->x + (pPlayer->hullRect.w)/10;

        pPlayer->canonRect.x = (int)pPlayer->x + 2*(pPlayer->hullRect.w)/3;
    }
    else
    {
        pPlayer->turretRect.x = (int)pPlayer->x + (pPlayer->hullRect.w)/3.7f;

        pPlayer->canonRect.x = (int)pPlayer->x + (pPlayer->hullRect.w)/3;
    }

    pPlayer->hitbox.x = pPlayer->hullRect.x;
    pPlayer->hitbox.y = pPlayer->hullRect.y;
    pPlayer->hitbox.w = pPlayer->hullRect.w;
    pPlayer->hitbox.h = pPlayer->hullRect.h;
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
    pPlayer->canonMode = 1;

    SDL_Surface *pSurface = IMG_Load("Resources/Sprite-tankHull.png");
    if (!pSurface) 
    {
        printf("Error loading Sprite-tankHull.png: %s\n", IMG_GetError());
        free(pPlayer);
        return NULL;
    }

    pPlayer->pRenderer = pRenderer;
    pPlayer->pHullTx = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    pSurface = IMG_Load("Resources/Sprite-tankTurret.png");
    if (!pSurface)
    {
        printf("Error loading Sprite-tankTurret.png: %s\n", IMG_GetError());
        free(pPlayer);
        return NULL;
    }

    pPlayer->pTurretTx = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    pSurface = IMG_Load("Resources/Sprite-tankBarrel.png");
    if (!pSurface)
    {
        printf("Error loading Sprite-tankBarrel.png: %s\n", IMG_GetError());
        free(pPlayer);
        return NULL;
    }

    pPlayer->pCanonTx = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    free(pSurface);

    if (!pPlayer->pHullTx) 
    {
        printf("Error creating player texture: %s\n", SDL_GetError());
        free(pPlayer);
        return NULL;
    }

    if (!pPlayer->pTurretTx) 
    {
        printf("Error creating turret texture: %s\n", SDL_GetError());
        free(pPlayer);
        return NULL;
    }
    
    if (!pPlayer->pCanonTx) 
    {
        printf("Error creating canon texture: %s\n", SDL_GetError());
        free(pPlayer);
        return NULL;
    }

    SDL_QueryTexture(pPlayer->pHullTx, NULL, NULL, &pPlayer->hullRect.w, &pPlayer->hullRect.h);
    SDL_QueryTexture(pPlayer->pTurretTx, NULL, NULL, &pPlayer->turretRect.w, &pPlayer->turretRect.h);
    SDL_QueryTexture(pPlayer->pCanonTx, NULL, NULL, &pPlayer->canonRect.w, &pPlayer->canonRect.h);

    pPlayer->hullRect.w = 77*1.2f;
    pPlayer->hullRect.h = 26*1.2f;

    pPlayer->turretRect.w = 49*1.2f;
    pPlayer->turretRect.h = 30*1.2f;   

    pPlayer->canonRect.w = 54*1.2f;
    pPlayer->canonRect.h = 15*1.2f;

    pPlayer->x = x - pPlayer->hullRect.w / 2.0f;
    pPlayer->y = y - pPlayer->hullRect.h / 2.0f;

    pPlayer->tankFlip = SDL_FLIP_NONE;

    updatePlayerRects(pPlayer);
    return pPlayer;
}

void moveLeft(Player *pPlayer)
{
    pPlayer->velX += -pPlayer->moveSpeed;
    pPlayer->tankFlip = SDL_FLIP_HORIZONTAL;
}

void moveRight(Player *pPlayer)
{
    pPlayer->velX += pPlayer->moveSpeed;
    pPlayer->tankFlip = SDL_FLIP_NONE;
}

void jump(Player *pPlayer)
{   
    if(pPlayer->isGrounded)
    {
        pPlayer->velY = -pPlayer->jumpForce;
        pPlayer->isGrounded = 0;
    }
}

void setCanonMode(Player *pPlayer, int mode)
{
    pPlayer->canonMode = mode;
}

int getCanonMode(Player *pPlayer)
{
    return pPlayer->canonMode;
}

float getXCord(Player *pPlayer)
{
    return pPlayer->canonRect.x;
}

float getYCord(Player *pPlayer)
{
    return pPlayer->canonRect.y;
}

float getCanonX(Player *pPlayer)
{
    return pPlayer->canonRect.x + (pPlayer->canonRect.w-5)*cos(pPlayer->canonAngle);
}

float getCanonY(Player *pPlayer)
{
    return pPlayer->canonRect.y + (pPlayer->canonRect.h)/2 + (pPlayer->canonRect.w-5)*sin(pPlayer->canonAngle);
}

float getAngle(Player *pPlayer)
{
    return pPlayer->canonAngle;
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

static void restrictCanonAngle(Player *pPlayer)
{
    if(pPlayer->turretFlip == SDL_FLIP_NONE)
    {
        if(pPlayer->canonAngle <= -1.2f) pPlayer->canonAngle = -1.2f;
        else if(pPlayer->canonAngle >= 0.7f) pPlayer->canonAngle = 0.7f;
    }
    else if(pPlayer->turretFlip == SDL_FLIP_HORIZONTAL)
    {
        if(pPlayer->canonAngle > 4.2f) pPlayer->canonAngle = 4.2f;
        else if(pPlayer->canonAngle < 2.4f && (pPlayer->canonAngle > 0.0f)) pPlayer->canonAngle = 2.4f;
    }

}

static void flipCanon(Player *pPlayer)
{
    if(pPlayer->targetAngle > -PI/2 && pPlayer->targetAngle < PI/2)
    {
        if(pPlayer->canonAngle <= 2.4f) 
        {
            pPlayer->turretFlip = SDL_FLIP_NONE;
        }
        else if(pPlayer->canonAngle >= 4.2f)
        {
            pPlayer->turretFlip = SDL_FLIP_NONE;
            pPlayer->canonAngle = -1.2f;
        }
    }
    else if(pPlayer->canonAngle >= 0.7f) pPlayer->turretFlip = SDL_FLIP_HORIZONTAL;
    else if(pPlayer->canonAngle <= -1.2f)
    {
        pPlayer->turretFlip = SDL_FLIP_HORIZONTAL;
        pPlayer->canonAngle = 4.2f;
    }
}

void updatePlayer(Player *pPlayer, Map *pMap)
{
    int mousePosx, mousePosy;
    float diffAngle;
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

    float dx = mousePosx - pPlayer->x - (pPlayer->hullRect.w)/2;
    float dy = mousePosy - pPlayer->y + (pPlayer->hullRect.h)/2;

    pPlayer->targetAngle = atan2(dy, dx);

    flipCanon(pPlayer);

    if(pPlayer->turretFlip == SDL_FLIP_HORIZONTAL)
    {
        if(pPlayer->targetAngle < 0) pPlayer->targetAngle += 2*PI;
    }
    diffAngle = pPlayer->targetAngle - pPlayer->canonAngle;

    if(fabs(diffAngle) > 0.05f)
    {
        if(diffAngle > 0) pPlayer->canonAngle += 0.05f;
        else pPlayer->canonAngle += -0.05f;
    }
    else pPlayer->canonAngle = pPlayer->targetAngle;
    restrictCanonAngle(pPlayer);
    
    if (pPlayer->x < 0) pPlayer->x = 0;
    if (pPlayer->x + pPlayer->hullRect.w > pPlayer->window_width)
        pPlayer->x = pPlayer->window_width - pPlayer->hullRect.w;

    if (pPlayer->y + pPlayer->hullRect.h > pPlayer->window_height) 
    {
        pPlayer->y = pPlayer->window_height - pPlayer->hullRect.h;
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

    SDL_RenderCopyEx(pPlayer->pRenderer, pPlayer->pHullTx, NULL, &pPlayer->hullRect, 0.0, NULL, pPlayer->tankFlip);
    SDL_RenderCopyEx(pPlayer->pRenderer, pPlayer->pCanonTx, NULL, &pPlayer->canonRect, pPlayer->canonAngle*180/3.141f, &canonCenter, SDL_FLIP_NONE);
    SDL_RenderCopyEx(pPlayer->pRenderer, pPlayer->pTurretTx, NULL, &pPlayer->turretRect, 0.0, NULL, pPlayer->turretFlip);
    
}

SDL_Rect getPlayerHitbox(Player *pPlayer)
{
    return pPlayer->hitbox;
}

SDL_Rect getPlayerRect(Player *pPlayer)
{
    return pPlayer->hullRect;
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
    if (pPlayer->pHullTx) SDL_DestroyTexture(pPlayer->pHullTx);
    if (pPlayer->pCanonTx) SDL_DestroyTexture(pPlayer->pCanonTx);
    if (pPlayer->pTurretTx) SDL_DestroyTexture(pPlayer->pTurretTx);
    free(pPlayer);
}