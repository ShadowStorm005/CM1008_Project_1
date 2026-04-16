#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "map.h"

struct platform {
    SDL_Renderer *pRenderer;
    SDL_Texture *pTexture;
    SDL_Rect rect;
    int active;
};

Platform *createPlatforms(SDL_Renderer *pRenderer, int window_width, int window_height, int *platformCount)
{
    *platformCount = 4;

    Platform *platforms = malloc(sizeof(Platform) * (*platformCount));
    if (!platforms) return NULL;

    SDL_Surface *surface = IMG_Load("Resources/PrimitivPlatform.png");
    if (!surface) {
        printf("Error loading PrimitivPlatform.png: %s\n", IMG_GetError());
        free(platforms);
        return NULL;
    }

    SDL_Texture *sharedTexture = SDL_CreateTextureFromSurface(pRenderer, surface);
    SDL_FreeSurface(surface);

    if (!sharedTexture) {
        printf("Error creating platform texture: %s\n", SDL_GetError());
        free(platforms);
        return NULL;
    }

    for (int i = 0; i < *platformCount; i++) {
        platforms[i].pRenderer = pRenderer;
        platforms[i].pTexture = sharedTexture;
        platforms[i].active = 1;
    }

    platforms[0].rect.x = 120;
    platforms[0].rect.y = window_height - 180;
    platforms[0].rect.w = 350;
    platforms[0].rect.h = 40;

    platforms[1].rect.x = 520;
    platforms[1].rect.y = window_height - 300;
    platforms[1].rect.w = 250;
    platforms[1].rect.h = 40;

    platforms[2].rect.x = 850;
    platforms[2].rect.y = window_height - 430;
    platforms[2].rect.w = 220;
    platforms[2].rect.h = 40;

    platforms[3].rect.x = 300;
    platforms[3].rect.y = window_height - 520;
    platforms[3].rect.w = 200;
    platforms[3].rect.h = 40;

    return platforms;
}

void drawPlatforms(Platform *platforms, int platformCount)
{
    for (int i = 0; i < platformCount; i++) {
        if (platforms[i].active) {
            SDL_RenderCopy(platforms[i].pRenderer, platforms[i].pTexture, NULL, &platforms[i].rect);

            SDL_SetRenderDrawColor(platforms[i].pRenderer, 0, 255, 0, 255);
            SDL_RenderDrawRect(platforms[i].pRenderer, &platforms[i].rect);
        }
    }
}

SDL_Rect getPlatformRect(Platform *platforms, int index)
{
    return platforms[index].rect;;
}

void destroyPlatforms(Platform *platforms, int platformCount)
{
    if (!platforms) return;

    if (platformCount > 0 && platforms[0].pTexture) {
        SDL_DestroyTexture(platforms[0].pTexture);
    }

    free(platforms);
}