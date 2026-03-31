#ifndef player_h
#define player_h

typedef struct player Player;

Player *createPlayer(float x, float y, SDL_Renderer *pRenderer, int window_width, int window_height);
#endif