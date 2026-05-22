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
    int explosionTimer;

    SDL_Texture *pTexture;

    SDL_Rect explosionRect;
    SDL_Rect explosionSrcRect;

    SDL_Renderer *pRenderer;
} Explosion;

void updateExplosionTexture(Explosion *pExplosion)
{

}

void updateExplosionRects(Explosion *pExplosion)
{
    pExplosion->explosionRect.x = pExplosion->x;
    pExplosion->explosionRect.y = pExplosion->y;
    updateExplosionTexture(pExplosion);
}

void initExplosionDefaults(Explosion *pExplosion)
{
    pExplosion->x = 100;
    pExplosion->y = 100;

    pExplosion->explosionRect.x = pExplosion->x;
    pExplosion->explosionRect.y = pExplosion->y;

    pExplosion->explosionRect.w = 112;
    pExplosion->explosionRect.h = 112;

    pExplosion->explosionSrcRect.x = 0;
    pExplosion->explosionSrcRect.y = 0;

    pExplosion->explosionSrcRect.w = 112;
    pExplosion->explosionSrcRect.h = 112;

    pExplosion->active = 1;
    pExplosion->explosionTimer = 600;
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

int isExplosionActive(Explosion *pExplosion)
{
    return pExplosion->active;
}

void activateExplosion(Explosion *pExplosion, int x, int y)
{
    pExplosion->x = x;
    pExplosion->y = y;
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
