#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "player.h"


#define WINDOW_WIDTH 1980
#define WINDOW_HEIGHT 1080


/*typedef struct{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Player *pPlayer;
} Game;*/


int main(int argv, char** args){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    Player *player = createPlayer(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, renderer, );

    

    bool isRunning = true;
    SDL_Event event;

    while(isRunning){
        while(SDL_PollEvent(&event)){
            switch (event.type)
            {
                case SDL_QUIT: isRunning = false;
            }
        }
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}