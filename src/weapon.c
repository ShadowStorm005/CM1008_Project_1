#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "player.h"
#include "weapon.h"
#include "map.h"
#include "physics.h"

#define BULLET_ASPECT 1

typedef struct projectile
{
    float x, y;
    float velX, velY;
    float angle;
    int window_width, window_height;
    int active;
    float speed;
    float gravity;

    SDL_Texture *pTexture;
    SDL_Renderer *pRenderer;

    SDL_Rect projectile_rect;
} Projectile;

Projectile *createProjectile(SDL_Renderer *pRenderer)
{
    Projectile *pProjectile = malloc(sizeof(Projectile));
    if(!pProjectile) return NULL;

    pProjectile->projectile_rect.x = (int)pProjectile->x;
    pProjectile->projectile_rect.y = (int)pProjectile->y;
    pProjectile->active = 0;
    pProjectile->gravity = 0.15f;

    SDL_Surface *pSurface = IMG_Load("Resources/bullet.png");
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

    return pProjectile;
}

int isActive(Projectile *pProjectile)
{
    return pProjectile->active;
}

void updateProjectileRect(Projectile *pProjectile)
{
    pProjectile->projectile_rect.x = (int)(pProjectile->x);
    pProjectile->projectile_rect.y = (int)(pProjectile->y - pProjectile->projectile_rect.h / 2);
}

void updateProjectile(Projectile *pProjectile, Map *pMap)
{
    pProjectile->velY += pProjectile->gravity;

    pProjectile->x += pProjectile->velX;
    pProjectile->y += pProjectile->velY;

    updateProjectileRect(pProjectile);

    checkForBulletCollision(pProjectile, pMap);

    pProjectile->angle = atan2(pProjectile->velY, pProjectile->velX);

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

void shoot(Projectile *pProjectile[], float x, float y, float angle)
{
    int mousePosx, mousePosy;
    //Uint32 buttons = SDL_GetMouseState(&mousePosx, &mousePosy);
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(!pProjectile[i]->active)
        {
            //bullet size
            pProjectile[i]->projectile_rect.w = 20*BULLET_ASPECT;
            pProjectile[i]->projectile_rect.h = 20;

            pProjectile[i]->active = 1;
            pProjectile[i]->x = x;
            pProjectile[i]->y = y;

            updateProjectileRect(pProjectile[i]);

            //float dx = mousePosx - x;
            //float dy = mousePosy - y;

            //pProjectile[i]->angle = atan2(dy, dx);
            pProjectile[i]->angle = angle;
            pProjectile[i]->speed = 16.0f;
            pProjectile[i]->velX = pProjectile[i]->speed * cos(pProjectile[i]->angle);
            pProjectile[i]->velY = pProjectile[i]->speed * sin(pProjectile[i]->angle);

            break;
        }
    }
}

void inactivateBullet(Projectile *pProjectile)
{
    pProjectile->active = 0;
}

SDL_Rect getBulletRect(Projectile *pProjectile)
{
    return pProjectile->projectile_rect;
}

void destroyProjectile(Projectile *pProjectile)
{
    if(!pProjectile) return;
    if(pProjectile->pTexture) SDL_DestroyTexture(pProjectile->pTexture);
    free(pProjectile);
}