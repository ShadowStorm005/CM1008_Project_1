#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "player.h"
#include "weapon.h"
#include "map.h"

struct Weapon
{
    float x, y;
    float velX, valY;
    float angle;
    int window_width, window_height;

    SDL_Texture *pTexture;
    SDL_Renderer *pRenderer;

    SDL_Rect player_rect;
};