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
#include "sound.h"

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

Projectile *createServerProjectile(void)
{
    Projectile *pProjectile = malloc(sizeof(Projectile));
    if(!pProjectile) return NULL;

    pProjectile->x = 0.0f;
    pProjectile->y = 0.0f;
    pProjectile->velX = 0.0f;
    pProjectile->velY = 0.0f;
    pProjectile->angle = 0.0f;
    pProjectile->active = 0;
    pProjectile->speed = 10.0f;
    pProjectile->gravity = 0.15f;
    pProjectile->pTexture = NULL;
    pProjectile->pRenderer = NULL;
    pProjectile->projectile_rect.x = (int)pProjectile->x;
    pProjectile->projectile_rect.y = (int)pProjectile->y;
    pProjectile->projectile_rect.w = BULLET_SIZE * BULLET_ASPECT;
    pProjectile->projectile_rect.h = BULLET_SIZE;
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

void updateProjectile(Projectile *pProjectile, Map *pMap, NetTile tileChanges[MAX_TILE_CHANGES], uint8_t *tileChangeCount, Sounds *sounds)
{
    pProjectile->velY += pProjectile->gravity;

    pProjectile->x += pProjectile->velX;
    pProjectile->y += pProjectile->velY;

    updateProjectileRect(pProjectile);

    checkForBulletCollision(pProjectile, pMap, tileChanges, tileChangeCount, sounds);

    pProjectile->angle = atan2(pProjectile->velY, pProjectile->velX);

    if ((pProjectile->x+pProjectile->projectile_rect.w) < 0 || pProjectile->x > WINDOW_WIDTH || pProjectile->y > WINDOW_HEIGHT)
    {
        pProjectile->active = 0;
    }
}

void setProjectileVar(Projectile *pProjectile, int active, float x, float y, float angle)
{
    pProjectile->active = active;
    pProjectile->x = x;
    pProjectile->y = y;
    pProjectile->angle = angle;
    pProjectile->projectile_rect.w = BULLET_SIZE * BULLET_ASPECT;
    pProjectile->projectile_rect.h = BULLET_SIZE;
}

void drawProjectile(Projectile *pProjectile)
{
    if(!pProjectile->active) return;
    SDL_Point center = {0, pProjectile->projectile_rect.h / 2};
    SDL_RenderCopyEx(pProjectile->pRenderer, pProjectile->pTexture, NULL /**/, &pProjectile->projectile_rect, pProjectile->angle*180/PI, &center, SDL_FLIP_NONE);
}

void shoot(Projectile *pProjectile[], int size, float speed, float x, float y, float angle)
{
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if(!pProjectile[i]->active)
        {
            //bullet size
            pProjectile[i]->projectile_rect.w = size*BULLET_ASPECT;
            pProjectile[i]->projectile_rect.h = size;

            pProjectile[i]->active = 1;
            pProjectile[i]->x = x;
            pProjectile[i]->y = y;

            updateProjectileRect(pProjectile[i]);
            pProjectile[i]->angle = angle;
            pProjectile[i]->speed = speed;
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

float getBulletX(Projectile *pProjectile)
{
    return pProjectile->x;
}

float getBulletY(Projectile *pProjectile)
{
    return pProjectile->y;
}

float getBulletAngle(Projectile *pProjectile)
{
    return pProjectile->angle;
}