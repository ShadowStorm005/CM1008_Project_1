#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include "player.h"
#include "map.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960

typedef struct {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Player *pPlayer;
    Platform *pPlatforms;
    int platformCount;
    SDL_Texture *pbackground;
} Game;

int initiate(Game *pGame);
void run(Game *pGame);
void closeGame(Game *pGame);
void handleInput(Game *pGame, const Uint8 *keystate);

int main(int argc, char **argv)
{
    Game game = {0};

    if (!initiate(&game)) {
        return 1;
    }

    run(&game);
    closeGame(&game);
    return 0;
}

int initiate(Game *pGame)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return 0;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 0;
    }

    pGame->pWindow = SDL_CreateWindow(
        "Tank Turtles",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if (!pGame->pWindow) {
        printf("Window Error: %s\n", SDL_GetError());
        closeGame(pGame);
        return 0;
    }

    pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!pGame->pRenderer) {
        printf("Renderer Error: %s\n", SDL_GetError());
        closeGame(pGame);
        return 0;
    }

    SDL_Surface *pbackground = IMG_Load("Resources/skybackground.png");
    if (!pbackground) {
        printf("Error loading skybackground.png: %s\n", IMG_GetError());
        free(pbackground);
        return 0;
    }

    pGame->pbackground = SDL_CreateTextureFromSurface(pGame->pRenderer, pbackground);
    free(pbackground);

    if (!pGame->pbackground) {
        printf("Error creating texture: %s\n", SDL_GetError());
        closeGame(pGame);
        return 0;
    }

    pGame->pPlayer = createPlayer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, pGame->pRenderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!pGame->pPlayer) {
        printf("Player creation failed\n");
        closeGame(pGame);
        return 0;
    }

    pGame->pPlatforms = createPlatforms(pGame->pRenderer, WINDOW_WIDTH, WINDOW_HEIGHT, &pGame->platformCount);
    if (!pGame->pPlatforms) {
        printf("Platform creation failed\n");
        closeGame(pGame);
        return 0;
    }

    return 1;
}

void run(Game *pGame)
{
    int close_requested = 0;
    SDL_Event event;

    while (!close_requested) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) 
            {
                close_requested = 1;
            } 
        }
        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        handleInput(pGame, keystate);

        updatePlayer(pGame->pPlayer, pGame->pPlatforms, pGame->platformCount);

        SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pGame->pRenderer);

        SDL_RenderCopy(pGame->pRenderer, pGame->pbackground, NULL, NULL);

        drawPlatforms(pGame->pPlatforms, pGame->platformCount);
        drawPlayer(pGame->pPlayer);

        SDL_RenderPresent(pGame->pRenderer);
        SDL_Delay(16);
    }
}

void handleInput(Game *pGame, const Uint8 *keystate)
{
    if(keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A])
    {
        moveLeft(pGame->pPlayer);
    }
    if(keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D])
    {
        moveRight(pGame->pPlayer);
    }
    if(keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W])
    {
        jump(pGame->pPlayer);
    }
}

void closeGame(Game *pGame)
{
    if (pGame->pPlayer) destroyPlayer(pGame->pPlayer);
    if (pGame->pPlatforms) destroyPlatforms(pGame->pPlatforms, pGame->platformCount);
    if (pGame->pbackground)SDL_DestroyTexture(pGame->pbackground); 
    if (pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if (pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);

    IMG_Quit();
    SDL_Quit();
}