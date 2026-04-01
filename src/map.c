#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include "map.h"


struct platform{ 
    int window_width,window_height;
    SDL_Renderer *pRenderer;
    SDL_Texture *pTexture;
    SDL_Rect rect;
};


Platform *createPlatform(SDL_Renderer *pRenderer, int window_width, int window_height)
{
    Platform *pPlatform = malloc(sizeof(struct platform));
    SDL_Surface *surface = IMG_Load("Resources/PrimitivPlatform.png");
    if(!surface)
    {
        printf("Error: %s\n",SDL_GetError());
        return NULL;
    }

    pPlatform->pRenderer = pRenderer;
    pPlatform->pTexture = SDL_CreateTextureFromSurface(pRenderer, surface);
    SDL_FreeSurface(surface);
    if(!pPlatform->pTexture)
    {
        printf("Error: %s\n",SDL_GetError());
        return NULL;
    }
    
    
    pPlatform->window_width = window_width;                           
    pPlatform->window_height = window_height;       

    SDL_QueryTexture(pPlatform->pTexture,NULL,NULL,&(pPlatform->rect.w),&(pPlatform->rect.h));

    
    pPlatform->rect.w = pPlatform->window_width/2;                         
    pPlatform->rect.h = pPlatform->window_height/16;                    // size of platform

    pPlatform->rect.x = pPlatform->window_width - pPlatform->window_width/2 - 200;            // location / make a dynamic function that makes the platform apper in the middle of the screen
    pPlatform->rect.y = pPlatform->window_height - pPlatform->window_height/4;
    //getStartValues(pPlatform);
    

    return pPlatform;
}

void drawPlatform(Platform *pPlatform){
    SDL_RenderCopy(pPlatform->pRenderer,pPlatform->pTexture,NULL,&(pPlatform->rect));
}

void destroyPlatform(Platform *pflatform)
{
    free(pflatform);
}

