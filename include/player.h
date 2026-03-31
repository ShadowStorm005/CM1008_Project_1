#ifndef player_h
#define player_h

typedef struct player Player;

Player *createPlayer(int x, int y, SDL_Renderer *pRenderer, int window_width, int window_height);
#endif