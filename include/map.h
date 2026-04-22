#ifndef MAP_H
#define MAP_H
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960
#define AMOUNT_OF_TILES_HORIZONTAL 80
#define AMOUNT_OF_TILES_VERTICAL 60
#define TILESIZE_PIXELS 16

#include <SDL.h>


typedef struct tile Tile;
typedef struct map Map;

Map *createMap(SDL_Renderer *pRenderer, int window_width, int window_height);
void drawTiles(Map *map);
SDL_Rect getTileRect(Map *map, int x, int y);
int isTileAktive(Map *tiles, int x, int y);
void destroyTiles(Map *map, int tilecount);

Tile createTile(int x, int y, int selectedTile);

#endif