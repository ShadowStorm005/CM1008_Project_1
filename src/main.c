#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "player.h"


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960


typedef struct{
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Player *pPlayer;
} Game;


int initiate(Game *pGame);
void run(Game *pGame);
void close(Game *pGame);


int main(int argv, char** args){
    Game game = {0};
    initiate(&game);
    run(&game);
    close(&game);

    /*bool isRunning = true;
    SDL_Event event;

    while(isRunning){
        while(SDL_PollEvent(&event)){
            switch (event.type)
            {
                case SDL_QUIT: isRunning = false;
            }
        }
        SDL_RenderClear(game.pRenderer);
        SDL_SetRenderDrawColor(game.pRenderer, 255, 0, 0, 255);
        drawPlayer(game.pPlayer);
        SDL_RenderPresent(game.pRenderer);
        SDL_Delay(1000/60-15);
    }
    SDL_DestroyRenderer(game.pRenderer);
    SDL_DestroyWindow(game.pWindow);
    SDL_Quit();*/

    return 0;
}

int initiate(Game *pGame)
{
    if(SDL_Init(SDL_INIT_EVERYTHING)!=0){
        printf("Error: %s\n",SDL_GetError());
        return 0;
    }
    pGame->pWindow = SDL_CreateWindow("Tank Turtles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if(!pGame->pWindow){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;
    }
    pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, 0);
    if(!pGame->pRenderer){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;    
    }
    pGame->pPlayer = createPlayer(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, pGame->pRenderer, 500, 200);
    if(!pGame->pPlayer){
        printf("Error: %s\n",SDL_GetError());
        close(pGame);
        return 0;
    }
    return 1;
}

void run(Game *pGame)
{
    int close_requested = 0;
    SDL_Event event;
    while(!close_requested)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type==SDL_QUIT) close_requested = 1;
            //else handleInput(pGame,&event);
        }
        SDL_RenderClear(pGame->pRenderer);
        SDL_SetRenderDrawColor(pGame->pRenderer, 255, 0, 0, 255);
        drawPlayer(pGame->pPlayer);
        SDL_RenderPresent(pGame->pRenderer);
        SDL_Delay(1000/60-15);
    }
}

void close(Game *pGame)
{
    if(pGame->pPlayer) destroyPlayer(pGame->pPlayer);
    if(pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if(pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);
    SDL_Quit();
}