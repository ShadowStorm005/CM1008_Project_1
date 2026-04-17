#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "player.h"
#include "weapon.h"
#include "map.h"

#define BULLET_ASPECT 11

typedef struct projectile
{
    float x, y;
    float velX, velY;
    float angle;
    int window_width, window_height;
    int active;

    SDL_Texture *pTexture;
    SDL_Renderer *pRenderer;

    SDL_Rect projectile_rect;
} Projectile;

Projectile *createProjectile(SDL_Renderer *pRenderer, int window_width, int window_height)
{
    Projectile *pProjectile = malloc(sizeof(Projectile));
    if(!pProjectile) return NULL;

    pProjectile->projectile_rect.x = (int)pProjectile->x;
    pProjectile->projectile_rect.y = (int)pProjectile->y;
    pProjectile->active = 0;

    pProjectile->window_height = window_height/5.0f;
    pProjectile->window_width = window_width/5.0f;

    SDL_Surface *pSurface = IMG_Load("Resources/bullets.png");
    if (!pSurface) {
        printf("Error loading bullets.png: %s\n", IMG_GetError());
        free(pProjectile);
        return NULL;
    }

    pProjectile->pRenderer = pRenderer;
    pProjectile->pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);

    if (!pProjectile->pTexture) {
        printf("Error creating bullet texture: %s\n", SDL_GetError());
        free(pProjectile);
        return NULL;
    }
    SDL_QueryTexture(pProjectile->pTexture, NULL, NULL, &pProjectile->projectile_rect.w, &pProjectile->projectile_rect.h);

    pProjectile->projectile_rect.w = 20*BULLET_ASPECT;
    pProjectile->projectile_rect.h = 20;

    return pProjectile;
}

int isActive(Projectile *pProjectile)
{
    return pProjectile->active;
}

void updateProjectile(Projectile *pProjectile)
{
    pProjectile->x += pProjectile->velX;
    pProjectile->y += pProjectile->velY;
    pProjectile->projectile_rect.x = (int)(pProjectile->x);
    pProjectile->projectile_rect.y = (int)(pProjectile->y - pProjectile->projectile_rect.h / 2);

    if ((pProjectile->x+pProjectile->projectile_rect.w) < 0 || pProjectile->x > WINDOW_WIDTH || pProjectile->y > WINDOW_HEIGHT)
    {
        pProjectile->active = 0;
    }
}

void drawProjectile(Projectile *pProjectile)
{
    if(!pProjectile->active) return;
    SDL_Point center = {0, pProjectile->projectile_rect.h / 2};
    SDL_RenderCopyEx(pProjectile->pRenderer, pProjectile->pTexture, NULL /**/, &pProjectile->projectile_rect, pProjectile->angle*180/3.141f, &center, SDL_FLIP_NONE);
}

void shoot(Projectile *pProjectile[], float x, float y, int mousePosx, int mousePosy)
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(!pProjectile[i]->active)
        {
            pProjectile[i]->active = 1;
            pProjectile[i]->x = x;
            pProjectile[i]->y = y;

            float dx = mousePosx - x;
            float dy = mousePosy - y;

            pProjectile[i]->angle = atan2(dy, dx);
            pProjectile[i]->velX = 8.0f * cos(pProjectile[i]->angle);
            pProjectile[i]->velY = 8.0f * sin(pProjectile[i]->angle);

            break;
        }
    }
}

void destroyProjectile(Projectile *pProjectile)
{
    if(!pProjectile) return;
    if(pProjectile->pTexture) SDL_DestroyTexture(pProjectile->pTexture);
    free(pProjectile);
}