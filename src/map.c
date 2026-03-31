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
    int window_width,window_height,renderAngle;
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
    pPlatform->window_width = window_width;
    pPlatform->window_height = window_height;
    SDL_QueryTexture(pPlatformImage->pTexture,NULL,NULL,&(pPlatform->rect.w),&(pPlatform->rect.h));
    int sizeFactor = rand()%8+1;
    pPlatform->rect.w/=sizeFactor;
    pPlatform->rect.h/=sizeFactor;
    getStartValues(pPlatform);
    pPlatform->renderAngle = 0;

    return pPlatform;
}
/*
static void getStartValues(Asteroid *pAsteroid){
    int angle;
    if(rand()%2){
        pAsteroid->x=rand()%pAsteroid->window_width-pAsteroid->rect.w/2;
        pAsteroid->y=-pAsteroid->rect.h;
        angle=rand()%90-45;
    }else{
        pAsteroid->y=rand()%pAsteroid->window_height-pAsteroid->rect.h/2;
        pAsteroid->x=-pAsteroid->rect.w;
        angle=rand()%90;
    }
    int v=rand()%8+5;
    pAsteroid->vx=v*sin(angle*2*M_PI/360);
    pAsteroid->vy=v*cos(angle*2*M_PI/360);
    pAsteroid->rect.x=pAsteroid->x;
    pAsteroid->rect.y=pAsteroid->y;
}

SDL_Rect getRectAsteroid(Asteroid *pAsteroid){
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






