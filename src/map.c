#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include "map.h"


struct platformImage{ 
    SDL_Renderer *pRenderer;
    SDL_Texture *pTexture;    
};

struct platform{ 
    float x, y;
    int window_width,window_height;
    SDL_Renderer *pRenderer;
    SDL_Texture *pTexture;
    SDL_Rect rect;
};


PlatformImage *createPlatformImage(SDL_Renderer *pRenderer)
{
    static PlatformImage* pPlatformImage = NULL;
    if(pPlatformImage==NULL)
    {
        pPlatformImage = malloc(sizeof(struct platformImage));
        SDL_Surface *surface = IMG_Load("Resources/PrimitivPlatform.png");
        if(!surface)
        {
            printf("Error: %s\n",SDL_GetError());
            return NULL;
        }

        pPlatformImage->pRenderer = pRenderer;
        pPlatformImage->pTexture = SDL_CreateTextureFromSurface(pRenderer, surface);
        SDL_FreeSurface(surface);
        if(!pPlatformImage->pTexture)
        {
            printf("Error: %s\n",SDL_GetError());
            return NULL;
        }
    }
    return pPlatformImage;
}

Platform *createPlatform(PlatformImage *pPlatformImage, int window_width, int window_height)
{
    Platform *pPlatform = malloc(sizeof(struct platform));
    pPlatform->pRenderer = pPlatformImage->pRenderer;
    pPlatform->pTexture = pPlatformImage->pTexture;
    pPlatform->window_width = window_width/2;                           //  Appers in the middle of the screen
    pPlatform->window_height = window_height - window_height*3/4;       

    SDL_QueryTexture(pPlatformImage->pTexture,NULL,NULL,&(pPlatform->rect.w),&(pPlatform->rect.h));

    
    pPlatform->rect.w/=window_width/2;
    pPlatform->rect.h/=window_height/4;
    //getStartValues(pPlatform);
    

    return pPlatform;
}
/*
static void getStartValues(Platform *pPlatform){
    
}

SDL_Rect getRectAsteroid(Platform *pPlatform){
    return pAsteroid->rect;
}
*/
void destroyPlatform(Platform *pflatform)
{
    free(pflatform);
}

void destroyPlatformImage(PlatformImage *pPlatformImage)
{
    SDL_DestroyTexture(pPlatformImage->pTexture);
}






