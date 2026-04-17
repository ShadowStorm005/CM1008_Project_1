#ifndef MAP_H
#define MAP_H

#include <SDL.h>


typedef struct map Map;

Map *createMap(SDL_Renderer *pRenderer, int window_width, int window_height);
void drawTiles(Map *map);
SDL_Rect getTileRect(Map *map, int index);
void destroyPlatforms(Map *map, int tilecount);

#endif