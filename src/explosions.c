#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "player.h"
#include "weapon.h"
#include "map.h"
#include "physics.h"
#include "server_creation_functions.h"

typedef struct explosion
{
    int x,y;
    int active;

    int serverTime;
    int explosionTimerEnd;
    int startTime;
    int explosionTimer;

    SDL_Texture *pTexture;

    SDL_Rect explosionRect;
    SDL_Rect explosionSrcRect;

    SDL_Renderer *pRenderer;
} Explosion;

/*
    if(pPlayer->serverTime < pPlayer->smokeTimerEnd)
    {
        int timeLeft = pPlayer->smokeTimerEnd - pPlayer->serverTime;

        if(timeLeft > pPlayer->smokeTimer * 0.9f) pPlayer->barrelSmokeSrcRect.y = 0;
        else if(timeLeft > pPlayer->smokeTimer * 0.75f) pPlayer->barrelSmokeSrcRect.y = 64;
        else if(timeLeft > pPlayer->smokeTimer * 0.55f) pPlayer->barrelSmokeSrcRect.y = 64*2;
        else if(timeLeft > pPlayer->smokeTimer * 0.35f) pPlayer->barrelSmokeSrcRect.y = 64*3;
        else if(timeLeft > pPlayer->smokeTimer * 0.15f) pPlayer->barrelSmokeSrcRect.y = 64*4;
        else if(timeLeft > 0) pPlayer->barrelSmokeSrcRect.y = 64*5;

        pPlayer->barrelSmokeRect.w = 220;
        pPlayer->barrelSmokeRect.h = 64;
    }
    else
    {
        pPlayer->barrelSmokeRect.w = 0;
        pPlayer->barrelSmokeRect.h = 0;
    }
*/

void updateExplosionTexture(Explosion *pExplosion)
{
    if (pExplosion->serverTime < (pExplosion->explosionTimerEnd))
    {
        int timeLeft = pExplosion->explosionTimerEnd - pExplosion->serverTime;
        if(timeLeft > pExplosion->explosionTimer * 0.92f) pExplosion->explosionSrcRect.x = 0;
        else if(timeLeft > pExplosion->explosionTimer * 0.85f) pExplosion->explosionSrcRect.x = 112;
        else if(timeLeft > pExplosion->explosionTimer * 0.78f) pExplosion->explosionSrcRect.x = 112*2;
        else if(timeLeft > pExplosion->explosionTimer * 0.70f) pExplosion->explosionSrcRect.x = 112*3;

        else if(timeLeft > pExplosion->explosionTimer * 0.60f)
        {
            pExplosion->explosionSrcRect.y = 112;
            pExplosion->explosionSrcRect.x = 0;
        }
        else if(timeLeft > pExplosion->explosionTimer * 0.45f) pExplosion->explosionSrcRect.x = 112;
        else if(timeLeft > pExplosion->explosionTimer * 0.20f) pExplosion->explosionSrcRect.x = 112*2;
        else if(timeLeft > 0) pExplosion->explosionSrcRect.x = 112*3;
    }
    else
    {
        pExplosion->explosionSrcRect.y = 0;
        pExplosion->explosionSrcRect.x = 0;
        pExplosion->active = 0;
    }
}

void updateExplosionRects(Explosion *pExplosion)
{
    pExplosion->explosionRect.x = pExplosion->x - pExplosion->explosionRect.w / 2;
    pExplosion->explosionRect.y = pExplosion->y - pExplosion->explosionRect.h / 2;
    updateExplosionTexture(pExplosion);
}

void initExplosionDefaults(Explosion *pExplosion)
{
    pExplosion->x = 0;
    pExplosion->y = 0;

    pExplosion->explosionRect.x = pExplosion->x;
    pExplosion->explosionRect.y = pExplosion->y;

    pExplosion->explosionRect.w = 112;
    pExplosion->explosionRect.h = 112;

    pExplosion->explosionSrcRect.x = 0;
    pExplosion->explosionSrcRect.y = 0;

    pExplosion->explosionSrcRect.w = 112;
    pExplosion->explosionSrcRect.h = 112;

    pExplosion->active = 0;
    pExplosion->explosionTimer = 750;
}

Explosion *createExplosion(SDL_Renderer *pRenderer)
{
    Explosion *pExplosion = malloc(sizeof(Explosion));
    if (!pExplosion) return NULL;

    SDL_Surface *pSurface = IMG_Load("Resources/Sprite-explosions.png");
    if (!pSurface) {
        printf("Error loading bullets.png: %s\n", IMG_GetError());
        free(pExplosion);
        return NULL;
    }

    pExplosion->pRenderer = pRenderer;
    pExplosion->pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);

    if (!pExplosion->pTexture) {
        printf("Error creating explosion texture: %s\n", SDL_GetError());
        free(pExplosion);
        return NULL;
    }
    SDL_QueryTexture(pExplosion->pTexture, NULL, NULL, &pExplosion->explosionRect.w, &pExplosion->explosionRect.h);

    initExplosionDefaults(pExplosion);
    updateExplosionRects(pExplosion);

    return pExplosion;
}

Explosion *createServerExplosion(void)
{
    Explosion *pExplosion = malloc(sizeof(Explosion));
    if(!pExplosion) return NULL;

    initExplosionDefaults(pExplosion);

    return pExplosion;
}

void receiveExplosionServerTime(Explosion *pExplosion, int serverTime)
{
    pExplosion->serverTime = serverTime;
}

int isExplosionActive(Explosion *pExplosion)
{
    return pExplosion->active;
}

int getExplosionCordX(Explosion *pExplosion)
{
    return pExplosion->x;
}

int getExplosionCordY(Explosion *pExplosion)
{  
    return pExplosion->y;
}

int getStartTime(Explosion *pExplosion)
{
    return pExplosion->startTime;
}

void activateExplosion(Explosion *pExplosion, int x, int y, int startTime)
{
    pExplosion->active = 1;
    pExplosion->x = x;
    pExplosion->y = y;
    pExplosion->startTime = startTime;
    pExplosion->explosionTimerEnd = startTime + pExplosion->explosionTimer;
    updateExplosionRects(pExplosion);
}

void drawExplosion(Explosion *pExplosion)
{
    SDL_RenderCopyEx(pExplosion->pRenderer, pExplosion->pTexture, &pExplosion->explosionSrcRect, &pExplosion->explosionRect, 0.0, NULL, SDL_FLIP_NONE);
}

void destroyExplosion(Explosion *pExplosion)
{
    if(!pExplosion) return;
    if(pExplosion->pTexture) SDL_DestroyTexture(pExplosion->pTexture);
    free(pExplosion);
}
