#include <SDL.h>
#include <SDL_image.h>
#include "player.h"


struct player {
    int health;
    float x_cord, y_cord;
    int window_width, window_height;
    SDL_Texture *pTexture;
    SDL_Renderer *pRenderer;
    SDL_Rect playerRect;
};


Player *createPlayer(float x, float y, SDL_Renderer *pRenderer, int window_width, int window_height)
{
    Player *pPlayer = malloc(sizeof(struct player));
    pPlayer->window_width = window_width;
    pPlayer->window_height = window_height;
    SDL_Surface *pSurface = IMG_Load("Resources/tank.png");
    if(!pSurface){
        printf("Error: %s\n",SDL_GetError());
        return NULL;
    }

    pPlayer->pRenderer = pRenderer;
    pPlayer->pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if(!pPlayer->pTexture){
        printf("Error: %s\n",SDL_GetError());
        return NULL;
    }

    SDL_QueryTexture(pPlayer->pTexture,NULL,NULL,&(pPlayer->playerRect.w),&(pPlayer->playerRect.h));
    pPlayer->playerRect.w /=1;
    pPlayer->playerRect.h /=1;                                                  // size of player
    //pPlayer->x_cord=x-pPlayer->playerRect.w/2;
    //pPlayer->y_cord=y-pPlayer->playerRect.h/2;
    pPlayer->playerRect.x = pPlayer->window_width;                            // where the player will start
    pPlayer->playerRect.y = pPlayer->window_height;
    return pPlayer;
}

void drawPlayer(Player *pPlayer){
    SDL_RenderCopy(pPlayer->pRenderer,pPlayer->pTexture,NULL,&(pPlayer->playerRect));
}

void destroyPlayer(Player *pPlayer){
    SDL_DestroyTexture(pPlayer->pTexture);
    free(pPlayer);
}