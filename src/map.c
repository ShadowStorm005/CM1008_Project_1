#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#define AMOUNT_OF_TILES_HORIZONTAL 80
#define AMOUNT_OF_TILES_VERTICAL 60
#define TILESIZE_PIXELS 16

struct map
{
    SDL_Renderer *pRenderer;
    SDL_Texture *pTexture;
    SDL_Rect rect;
    int tileMap[AMOUNT_OF_TILES_HORIZONTAL][AMOUNT_OF_TILES_VERTICAL];
};


Map *createMap(SDL_Renderer *pRenderer, int window_width, int window_height)
{
    Map *map = malloc(sizeof(Map));
    if(!map) return NULL;

    map->pRenderer = pRenderer;

    SDL_Surface *surface = IMG_Load("Resources/foundation.png");
    if (!surface) {
        printf("Error loading foundation.png: %s\n", IMG_GetError());
        free(map);
        return NULL;
    }

    map->pTexture = SDL_CreateTextureFromSurface(pRenderer, surface);
    SDL_FreeSurface(surface);

    if (!map->pTexture) {
        printf("Error creating platform texture: %s\n", SDL_GetError());
        free(map);
        return NULL;
    }

    for (int x = 0; x < AMOUNT_OF_TILES_HORIZONTAL; x++) 
    {
        for (int y = 0; y < AMOUNT_OF_TILES_VERTICAL; y++) 
        {

            if (y >= 30)                                        // bestämmer "hur kartan ska se ut"
            {                              
                map->tileMap[x][y] = 1; 
                
            } else {
                map->tileMap[x][y] = 0;                     // arghhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh (detta var svårt)
            }
            

        }
    }
    return map;
}

void drawTiles(Map *tiles)
{
    SDL_Rect Select_Tile_1;
    Select_Tile_1.x = 0;
    Select_Tile_1.y = 0;
    Select_Tile_1.w = TILESIZE_PIXELS;
    Select_Tile_1.h = TILESIZE_PIXELS;

    SDL_Rect tile[AMOUNT_OF_TILES_HORIZONTAL][AMOUNT_OF_TILES_VERTICAL];
    for (int x = 0; x < AMOUNT_OF_TILES_HORIZONTAL; x++)
    {
        for(int y = 0; y < AMOUNT_OF_TILES_VERTICAL; y++)
        {
            tile[x][y].x = x*TILESIZE_PIXELS;
            tile[x][y].y = y*TILESIZE_PIXELS;
            tile[x][y].w = TILESIZE_PIXELS;
            tile[x][y].h = TILESIZE_PIXELS;
        }
    }

    for (int x = 0; x < AMOUNT_OF_TILES_HORIZONTAL; x++)
    {
        
        for(int y = 0; y < AMOUNT_OF_TILES_VERTICAL; y++)
        {
            switch (tiles->tileMap[x][y])
            {
            case 1:
                SDL_RenderCopy(tiles->pRenderer, tiles->pTexture, &Select_Tile_1, &tile[x][y] );
                break;
            }
        }
        
    
    }
}

SDL_Rect getTileRect(Map *tiles, int index)
{
    return tiles[index].rect;
}

void destroyPlatforms(Map *tiles, int tilecount)
{
    if (!tiles) return;

    if (tilecount > 0 && tiles[0].pTexture) {
        SDL_DestroyTexture(tiles[0].pTexture);
    }

    free(tiles);
}