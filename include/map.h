#ifndef MAP_H
#define MAP_H
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960

#include <SDL.h>

typedef struct platform Platform;

Platform *createPlatforms(SDL_Renderer *pRenderer, int window_width, int window_height, int *platformCount);
void drawPlatforms(Platform *platforms, int platformCount);
SDL_Rect getPlatformRect(Platform *platforms, int index);
void destroyPlatforms(Platform *platforms, int platformCount);

#endif