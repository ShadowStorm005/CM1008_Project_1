#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "player.h"
#include "weapon.h"
#include "map.h"
#include "physics.h"
#include "server_creation_functions.h"

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
    int tankSkin;

    int window_width, window_height;

    float canonAngle;
    float targetAngle;

    SDL_Texture *pHullTx;
    SDL_Texture *pCanonTx;
    SDL_Texture *pTurretTx;

    SDL_Renderer *pRenderer;

    SDL_Rect hitbox;

    SDL_Rect hullRect;
    SDL_Rect hullSrcRect;

    SDL_Rect canonRect;
    SDL_Rect canonSrcRect;

    SDL_Rect turretRect;
    SDL_Rect turretSrcRect;

    SDL_RendererFlip tankFlip;
    SDL_RendererFlip turretFlip;
};

void updatePlayerSkin(Player *pPlayer)
{
    switch (pPlayer->tankSkin)
    {
    case SKIN_SWEDEN:
        pPlayer->hullSrcRect.y = 0;
        pPlayer->turretSrcRect.y = 0;
        pPlayer->canonSrcRect.y = 0;
        break;
    case SKIN_DENMARK:
        pPlayer->hullSrcRect.y = 52*pPlayer->tankSkin;
        pPlayer->turretSrcRect.y = 40*pPlayer->tankSkin;
        pPlayer->canonSrcRect.y = 45*pPlayer->tankSkin/* +15*tankCanon */;
        break;
    case SKIN_DEUTSCH:
        pPlayer->hullSrcRect.y = 52*pPlayer->tankSkin;
        pPlayer->turretSrcRect.y = 40*pPlayer->tankSkin;
        pPlayer->canonSrcRect.y = 45*pPlayer->tankSkin/* +15*tankCanon */;      
        break;
    case SKIN_RUSSIA:
        pPlayer->hullSrcRect.y = 52*pPlayer->tankSkin;
        pPlayer->turretSrcRect.y = 40*pPlayer->tankSkin;
        pPlayer->canonSrcRect.y = 45*pPlayer->tankSkin/* +15*tankCanon */;
        break;
    }
}

void updatePlayerRects(Player *pPlayer)
{
    updatePlayerSkin(pPlayer);

    pPlayer->hullRect.x = (int)pPlayer->x;
    pPlayer->hullRect.y = (int)pPlayer->y;

    pPlayer->turretRect.y = (int)pPlayer->y - pPlayer->turretRect.h;
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

static void initPlayerDefaults(Player *pPlayer, float x, float y, int window_width, int window_height)
{
    pPlayer->window_width = window_width;
    pPlayer->window_height = window_height;
    pPlayer->health = 100;
    pPlayer->tankSkin = SKIN_SWEDEN;
    pPlayer->canFire = 1;

    pPlayer->velX = 0.0f;
    pPlayer->velY = 0.0f;
    pPlayer->moveSpeed = 2.0f;
    pPlayer->jumpForce = 12.0f;
    pPlayer->gravity = 0.7f;
    pPlayer->isGrounded = 0;
    pPlayer->isTouchingWall = 0;
    pPlayer->canonMode = 1;

    pPlayer->hullRect.w = 77*1.2f;
    pPlayer->hullRect.h = 26*1.2f;

    pPlayer->hullSrcRect.x = 0;
    pPlayer->hullSrcRect.y = 0; //26

    pPlayer->hullSrcRect.w = 77;
    pPlayer->hullSrcRect.h = 26;

    pPlayer->turretRect.w = 49*1.2f;
    pPlayer->turretRect.h = 40*1.2f; 

    pPlayer->turretSrcRect.x = 0;
    pPlayer->turretSrcRect.y = 0; // 40 

    pPlayer->turretSrcRect.w = 49;
    pPlayer->turretSrcRect.h = 40; 

    pPlayer->canonRect.w = 54*1.2f;
    pPlayer->canonRect.h = 15*1.2f;

    pPlayer->canonSrcRect.x = 0;
    pPlayer->canonSrcRect.y = 0;

    pPlayer->canonSrcRect.w = 54;
    pPlayer->canonSrcRect.h = 15;

    pPlayer->x = x - pPlayer->hullRect.w / 2.0f;
    pPlayer->y = y - pPlayer->hullRect.h / 2.0f;

    pPlayer->tankFlip = SDL_FLIP_NONE;
}

Player *createPlayer(float x, float y, SDL_Renderer *pRenderer, int window_width, int window_height)
{
    Player *pPlayer = malloc(sizeof(struct player));
    if (!pPlayer) return NULL;

    SDL_Surface *pSurface = IMG_Load("Resources/Sprite-tankHulls.png");
    if (!pSurface) 
    {
        printf("Error loading Sprite-tankHulls.png: %s\n", IMG_GetError());
        free(pPlayer);
        return NULL;
    }

    pPlayer->pRenderer = pRenderer;
    pPlayer->pHullTx = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    pSurface = IMG_Load("Resources/Sprite-tankTurrets.png");
    if (!pSurface)
    {
        printf("Error loading Sprite-tankTurrets.png: %s\n", IMG_GetError());
        free(pPlayer);
        return NULL;
    }

    pPlayer->pTurretTx = SDL_CreateTextureFromSurface(pRenderer, pSurface);

    pSurface = IMG_Load("Resources/Sprite-barrels.png");
    if (!pSurface)
    {
        printf("Error loading Sprite-barrels.png: %s\n", IMG_GetError());
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

    initPlayerDefaults(pPlayer, x, y, window_width, window_height);

    updatePlayerRects(pPlayer);
    return pPlayer;
}

Player *createServerPlayer(float x, float y, int window_width, int window_height)
{
    Player *pPlayer = malloc(sizeof(struct player));
    if (!pPlayer) return NULL;

    initPlayerDefaults(pPlayer, x, y, window_width, window_height);
    return pPlayer;
}

void changePlayerSkin(Player *pPlayer, int skin)
{
    pPlayer->tankSkin = skin;
    updatePlayerRects(pPlayer);
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

float getPlayerX(Player *pPlayer)
{
    return pPlayer->x;
}

float getPlayerY(Player *pPlayer)
{
    return pPlayer->y;
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

float getBulletSpeed(Player *pPlayer)
{
    switch(pPlayer->canonMode)
    {
    case 1:
        return 10.0f;
        break;
    case 2:
        return 16.0f;
        break;
    default:
        return 10.0f;
        break;
    }
}

int getBulletSize(Player *pPlayer)
{
    switch(pPlayer->canonMode)
    {
    case 1:
        return 30;
        break;
    case 2:
        return 15;
        break;
    default:
        return 30;
        break;
    }
}

void drawCircle(SDL_Renderer * renderer, int centerX, int centerY, float rad, int opacity)
{
    int outline = 70;
    int fill = 255;
    for(int x = -rad; x <= rad; x++)
    {
        for(int y = -rad; y <= rad; y++)
        {
            float dist = x*x + y*y;
            float rad2 = rad * rad;
            if(fabsf(dist - rad2) < rad) 
            {
                SDL_SetRenderDrawColor(renderer, outline, outline, outline, opacity);
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
            else if(dist <= rad2) 
            {
                SDL_SetRenderDrawColor(renderer, fill, fill, fill, opacity);
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

void drawTrajectory(Player *pPlayer, float initialSpeed, Map *pMap)
{
    if(pPlayer->velX || pPlayer->velX) return;
    
    float x = getCanonX(pPlayer);
    float y = getCanonY(pPlayer);

    float vx = initialSpeed * cos(pPlayer->canonAngle);
    float vy = initialSpeed * sin(pPlayer->canonAngle);
    float dt = 0.1f;
    float rad = 3.0f;

    for(int i = 0; i < 255; i++)
    {
        float prevX = x;
        float prevY = y;

        vy += PROJECTILE_GRAVITY * dt;
        x += vx * dt;
        y += vy * dt;
        int x1 = (int)prevX, x2 = (int)x, y1 = (int)prevY, y2 = (int)y;
        SDL_SetRenderDrawBlendMode(pPlayer->pRenderer, SDL_BLENDMODE_BLEND);
        if((i+1)%20 == 0) 
            {
                for (int i = 0; i < AMOUNT_OF_TILES_HORIZONTAL; i++)
                {
                    for (int j = 0; j < AMOUNT_OF_TILES_VERTICAL; j++)
                    {
                        SDL_Rect tileRect = getTileRect(pMap, i, j);
                        if (isTileActive(pMap, i, j))
                            if(SDL_IntersectRectAndLine(&tileRect, &x1, &y1, &x2, &y2)) return;
                    }
                }
                drawCircle(pPlayer->pRenderer, (int)x, (int)y, rad, 255-i);
            }
    }
}

void setTriggerState(Player *pPlayer, int enable)
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

void steerCanon(Player *pPlayer, int mousePosX, int mousePosY)
{
    float diffAngle;
    float dx = mousePosX - pPlayer->x - (pPlayer->hullRect.w)/2;
    float dy = mousePosY - pPlayer->y + (pPlayer->hullRect.h)/2;

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
}

void updatePlayer(Player *pPlayer, Map *pMap, int mouseX, int mouseY)
{
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

    SDL_RenderCopyEx(pPlayer->pRenderer, pPlayer->pHullTx, &pPlayer->hullSrcRect, &pPlayer->hullRect, 0.0, NULL, pPlayer->tankFlip);
    SDL_RenderCopyEx(pPlayer->pRenderer, pPlayer->pCanonTx, &pPlayer->canonSrcRect, &pPlayer->canonRect, pPlayer->canonAngle*180/3.141f, &canonCenter, SDL_FLIP_NONE);
    SDL_RenderCopyEx(pPlayer->pRenderer, pPlayer->pTurretTx, &pPlayer->turretSrcRect, &pPlayer->turretRect, 0.0, NULL, pPlayer->turretFlip);
    
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
    if (pPlayer->x < x) pPlayer->tankFlip = SDL_FLIP_NONE;
    else if (pPlayer->x > x) pPlayer->tankFlip = SDL_FLIP_HORIZONTAL;
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