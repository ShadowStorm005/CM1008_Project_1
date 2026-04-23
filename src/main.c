#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include "player.h"
#include "weapon.h"
#include "map.h"
#include "physics.h"

//window width & height moved to map.h

typedef struct {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Player *pPlayer;
    Platform *pPlatforms;
    int platformCount;
    SDL_Texture *pbackground;
    Projectile *pProjectile[MAX_BULLETS];
    bool inGameMenu;
    SDL_Texture *resumeButton;
    SDL_Texture *settingsButton;
    SDL_Texture *exitGameButton;
    SDL_Texture *backButton;
    SDL_Texture *newGameButton;
} Game;

int initiate(Game *pGame);
void menu(Game *pGame, SDL_Renderer* pRenderer, SDL_Event *event, SDL_Texture* background, SDL_Texture* newGame, SDL_Texture* settings, SDL_Texture* exitGameButton);
void settingsMenu(Game *pGame, SDL_Renderer* pRenderer, SDL_Event *event, bool *settings, SDL_Texture* background, SDL_Texture* backButton);
void inGameSettingsMenu(Game *pGame, SDL_Renderer* pRenderer, SDL_Event *event, bool *settings, SDL_Texture* backButton);
void inGameMenu(Game *pGame, SDL_Renderer* pRenderer, SDL_Event *event, bool *inGameMenu, SDL_Texture* resumeButton, SDL_Texture* settingsButton, SDL_Texture* exitGameButton);    
void run(Game *pGame);
void closeGame(Game *pGame);
void handleInput(Game *pGame, const Uint8 *keystate, bool *pInGameMenu);

int main(int argc, char **argv)
{
    Game game = {0};

    if (!initiate(&game)) 
    {
        return 1;
    }
    SDL_Event event = {0};
    menu(&game, game.pRenderer, &event, game.pbackground, IMG_LoadTexture(game.pRenderer, "Resources/firsttank.png"), IMG_LoadTexture(game.pRenderer, "Resources/tank.png"), IMG_LoadTexture(game.pRenderer, "Resources/PrimitivPlatform.png"));
    run(&game);
    closeGame(&game);
    return 0;
}

int initiate(Game *pGame)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
    {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return 0;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) 
    {
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

    if (!pGame->pWindow) 
    {
        printf("Window Error: %s\n", SDL_GetError());
        closeGame(pGame);
        return 0;
    }

    pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!pGame->pRenderer) 
    {
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

    pGame->resumeButton = IMG_LoadTexture(pGame->pRenderer, "Resources/firsttank.png");
    if (!pGame->resumeButton) {
        printf("Error loading resumeButton.png: %s\n", IMG_GetError());
        free(pGame->resumeButton);
        return 0;
    }

    pGame->newGameButton = IMG_LoadTexture(pGame->pRenderer, "Resources/firsttank.png");
    if (!pGame->newGameButton) {
        printf("Error loading newGame.png: %s\n", IMG_GetError());
        free(pGame->newGameButton);
        return 0;
    }

    pGame->settingsButton = IMG_LoadTexture(pGame->pRenderer, "Resources/tank.png");
    if (!pGame->settingsButton) {
        printf("Error loading settings.png: %s\n", IMG_GetError());
        free(pGame->settingsButton);
        return 0;
    }

    pGame->exitGameButton = IMG_LoadTexture(pGame->pRenderer, "Resources/PrimitivPlatform.png");
    if (!pGame->exitGameButton) {
        printf("Error loading exitGame.png: %s\n", IMG_GetError());
        free(pGame->exitGameButton);
        return 0;
    }

    pGame->backButton = IMG_LoadTexture(pGame->pRenderer, "Resources/bullets.png");
    if (!pGame->backButton) {
        printf("Error loading backButton.png: %s\n", IMG_GetError());
        free(pGame->backButton);
        return 0;
    }

    SDL_Texture* resumeGameButton = IMG_LoadTexture(pGame->pRenderer, "Resources/firsttank.png");
    if (!resumeGameButton) {
        printf("Error loading resumeGame.png: %s\n", IMG_GetError());
        free(resumeGameButton);
        return 0;
    }

    if (!pGame->pbackground) {
        printf("Error creating texture: %s\n", SDL_GetError());
        closeGame(pGame);
        return 0;
    }

    pGame->pPlayer = createPlayer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, pGame->pRenderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!pGame->pPlayer) 
    {
        printf("Player creation failed\n");
        closeGame(pGame);
        return 0;
    }

    pGame->pPlatforms = createPlatforms(pGame->pRenderer, WINDOW_WIDTH, WINDOW_HEIGHT, &pGame->platformCount);
    if (!pGame->pPlatforms) 
    {
        printf("Platform creation failed\n");
        closeGame(pGame);
        return 0;
    }
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        pGame->pProjectile[i] = createProjectile(pGame->pRenderer);
        if (!pGame->pProjectile[i])
        {
            printf("Projectile creation failed\n");
            closeGame(pGame);
            return 0;
        }
    }

    return 1;
}

void run(Game *pGame)
{
    int close_requested = 0;
    SDL_Event event;

    while (!close_requested) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                close_requested = 1;
            } 
        }
        const Uint8 *keystate = SDL_GetKeyboardState(NULL);

        handleInput(pGame, keystate, &pGame->inGameMenu);

        updatePlayer(pGame->pPlayer, pGame->pPlatforms, pGame->platformCount);
        for(int i = 0; i < MAX_BULLETS; i++)
        {
            if(isActive(pGame->pProjectile[i]))
            {
                updateProjectile(pGame->pProjectile[i]);
            }
        }

        SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pGame->pRenderer);

        SDL_RenderCopy(pGame->pRenderer, pGame->pbackground, NULL, NULL);

        drawPlatforms(pGame->pPlatforms, pGame->platformCount);
        drawPlayer(pGame->pPlayer);
        for(int i = 0; i < MAX_BULLETS; i++)
        {
            if(isActive(pGame->pProjectile[i]))
            {
                drawProjectile(pGame->pProjectile[i]);
            }
        }

        SDL_RenderPresent(pGame->pRenderer);
        SDL_Delay(16);
    }
}

void handleInput(Game *pGame, const Uint8 *keystate, bool *pInGameMenu)
{
    if(keystate[SDL_SCANCODE_ESCAPE])
    {
        pGame->inGameMenu = true;
        while(pGame->inGameMenu == true)
        {
            inGameMenu(pGame, pGame->pRenderer, &(SDL_Event){0}, &(pGame->inGameMenu), IMG_LoadTexture(pGame->pRenderer, "Resources/firsttank.png"), IMG_LoadTexture(pGame->pRenderer, "Resources/tank.png"), IMG_LoadTexture(pGame->pRenderer, "Resources/PrimitivPlatform.png"));
        }
    }
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
    /* The future functionality to change gun/canon needs function and stuff from player.c
    if(keystate[SDL_SCANCODE_1])
    {

    }
    if(keystate[SDL_SCANCODE_2])
    {

    }
    */
    if(keystate[SDL_SCANCODE_SPACE] || SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1))
    {
        if(canShoot(pGame->pPlayer))
        {
            enableTrigger(pGame->pPlayer, 0);
            shoot(pGame->pProjectile, getXCord(pGame->pPlayer), getYCord(pGame->pPlayer));
        }
    }
    else
    {
        enableTrigger(pGame->pPlayer, 1);
    }
}

void closeGame(Game *pGame)
{
    if (pGame->pPlayer) destroyPlayer(pGame->pPlayer);
    if (pGame->pPlatforms) destroyPlatforms(pGame->pPlatforms, pGame->platformCount);
    if (pGame->pbackground)SDL_DestroyTexture(pGame->pbackground); 
    for(int i = 0; i < MAX_BULLETS; i++)
    {
        if (pGame->pProjectile[i]) destroyProjectile(pGame->pProjectile[i]);
    }
    if (pGame->pRenderer) SDL_DestroyRenderer(pGame->pRenderer);
    if (pGame->pWindow) SDL_DestroyWindow(pGame->pWindow);

    IMG_Quit();
    SDL_Quit();
}

void menu(Game *pGame, SDL_Renderer* pRenderer, SDL_Event *event, SDL_Texture* background, SDL_Texture* newGameButton, SDL_Texture* settingsButton, SDL_Texture* exitGameButton)
{
    int Mx = 0;
    int My = 0;
    bool play = false;
    bool settings = false;
    bool exit = false;
    
    SDL_Event e;
    bool waiting = true;

    SDL_Rect background_rect;
    background_rect.x = 0;   
    background_rect.y = 0;   
    background_rect.w = 1280;
    background_rect.h = 960; 
    
    SDL_Rect newGame_Rect;
    newGame_Rect.x = 590;
    newGame_Rect.y = 400;
    newGame_Rect.w = 50;
    newGame_Rect.h = 25;

    SDL_Rect settings_Rect;
    settings_Rect.x = 590;
    settings_Rect.y = 450;
    settings_Rect.w = 50;
    settings_Rect.h = 25;
    
    SDL_Rect exit_Rect;
    exit_Rect.x = 590;
    exit_Rect.y = 500;
    exit_Rect.w = 50;
    exit_Rect.h = 25;

    SDL_RenderCopy(pRenderer, background, NULL, &background_rect);
    SDL_RenderCopy(pRenderer, newGameButton, NULL, &(SDL_Rect){590, 400, 50, 25});
    SDL_RenderCopy(pRenderer, settingsButton, NULL, &(SDL_Rect){590, 450, 50, 25});
    SDL_RenderCopy(pRenderer, exitGameButton, NULL, &(SDL_Rect){590, 500, 50, 25});
    SDL_RenderPresent(pRenderer);

    while(!play && !exit && !settings)
    {
        SDL_GetMouseState(&Mx, &My);

        while(SDL_PollEvent(event))
        {
            if(event->type == SDL_QUIT)
            {
                exit = true;
                break;
            }
            if(event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
            {
                if(Mx >= newGame_Rect.x && Mx <= newGame_Rect.x + newGame_Rect.w && My >= newGame_Rect.y && My <= newGame_Rect.y + newGame_Rect.h)
                {
                    play = true;
                }
                else if(Mx >= settings_Rect.x && Mx <= settings_Rect.x + settings_Rect.w && My >= settings_Rect.y && My <= settings_Rect.y + settings_Rect.h)
                {
                    settings = true;
                    settingsMenu(pGame, pRenderer, event, &settings, background, IMG_LoadTexture(pRenderer, "Resources/bullets.png"));
                }
                else if(Mx >= exit_Rect.x && Mx <= exit_Rect.x + exit_Rect.w && My >= exit_Rect.y && My <= exit_Rect.y + exit_Rect.h)
                {
                    exit = true;
                    closeGame(pGame);
                }
            }
        }
            SDL_RenderClear(pRenderer);

        SDL_RenderCopy(pRenderer, background, NULL, &background_rect);
        SDL_RenderCopy(pRenderer, newGameButton, NULL, &newGame_Rect);
        SDL_RenderCopy(pRenderer, settingsButton, NULL, &settings_Rect);
        SDL_RenderCopy(pRenderer, exitGameButton, NULL, &exit_Rect);

        SDL_RenderPresent(pRenderer);

        SDL_Delay(16);
    }

    while (waiting)
    {
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_MOUSEBUTTONUP)
            {
                waiting = false;
            }
        }
    }

    if (newGameButton) SDL_DestroyTexture(newGameButton);
    if (settingsButton) SDL_DestroyTexture(settingsButton);
    if (exitGameButton) SDL_DestroyTexture(exitGameButton);
}

void settingsMenu(Game *pGame, SDL_Renderer* pRenderer, SDL_Event *event, bool *settings, SDL_Texture* background, SDL_Texture* backButton)
{
    int Mx = 0;
    int My = 0;
    bool back = false;

    SDL_Event e;
    bool waiting = true;

    SDL_Rect background_rect;
    background_rect.x = 0;   
    background_rect.y = 0;   
    background_rect.w = 1280;
    background_rect.h = 960; 

    SDL_Rect backButton_Rect;
    backButton_Rect.x = 590;
    backButton_Rect.y = 500;
    backButton_Rect.w = 50;
    backButton_Rect.h = 25;

    SDL_RenderCopy(pRenderer, background, NULL, &background_rect);
    SDL_RenderCopy(pRenderer, backButton, NULL, &backButton_Rect);
    SDL_RenderPresent(pRenderer);

    while(!back)
    {
        SDL_GetMouseState(&Mx, &My);

        while(SDL_PollEvent(event))
        {
            if(event->type == SDL_QUIT)
            {
                back = true;
                break;
            }
            if(event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
            {
                if(Mx >= backButton_Rect.x && Mx <= backButton_Rect.x + backButton_Rect.w && My >= backButton_Rect.y && My <= backButton_Rect.y + backButton_Rect.h)
                {
                    back = true;
                }
            }
        }
        SDL_RenderClear(pRenderer);

        SDL_RenderCopy(pRenderer, background, NULL, &background_rect);
        SDL_RenderCopy(pRenderer, backButton, NULL, &backButton_Rect);

        SDL_RenderPresent(pRenderer);

        SDL_Delay(16);
    }

    while (waiting)
    {
        while(SDL_PollEvent(event))
        {
            if(event->type == SDL_MOUSEBUTTONUP)
            {
                waiting = false;
            }
        }
    }
    *settings = false;
    if (backButton) SDL_DestroyTexture(backButton);
}

void inGameSettingsMenu(Game *pGame, SDL_Renderer* pRenderer, SDL_Event *event, bool *settings, SDL_Texture* backButton)
{
    int Mx = 0;
    int My = 0;
    bool back = false;

    bool waiting = true;

    SDL_Rect backButton_Rect;
    backButton_Rect.x = 590;
    backButton_Rect.y = 500;
    backButton_Rect.w = 50;
    backButton_Rect.h = 25;

    while(!back)
    {
        SDL_GetMouseState(&Mx, &My);

        while(SDL_PollEvent(event))
        {
            if(event->type == SDL_QUIT)
            {
                back = true;
                break;
            }
            if(event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
            {
                if(Mx >= backButton_Rect.x && Mx <= backButton_Rect.x + backButton_Rect.w && My >= backButton_Rect.y && My <= backButton_Rect.y + backButton_Rect.h)
                {
                    back = true;
                }
            }
        }
        SDL_RenderClear(pRenderer);
        SDL_RenderCopy(pRenderer, pGame->pbackground, NULL, NULL);
        drawPlatforms(pGame->pPlatforms, pGame->platformCount);
        drawPlayer(pGame->pPlayer);
        for(int i = 0; i < MAX_BULLETS; i++)
        { 
            if(isActive(pGame->pProjectile[i]))
            {
                drawProjectile(pGame->pProjectile[i]);
            }
        }

        SDL_RenderCopy(pRenderer, backButton, NULL, &backButton_Rect);

        SDL_RenderPresent(pRenderer);

        SDL_Delay(16);
    }

    while (waiting)
    {
        while(SDL_PollEvent(event))
        {
            if(event->type == SDL_MOUSEBUTTONUP)
            {
                waiting = false;
            }
        }
    }
    *settings = false;
    if (backButton) SDL_DestroyTexture(backButton);
}

void inGameMenu(Game *pGame, SDL_Renderer* pRenderer, SDL_Event *event, bool *inGameMenu, SDL_Texture* resumeButton, SDL_Texture* settingsButton, SDL_Texture* exitGameButton)
{
    int Mx = 0;
    int My = 0;
    bool resume = false;
    bool settings = false;
    bool exit = false;
    bool waiting = true;

    SDL_Event e;

    SDL_Rect resumeGame_Rect;
    resumeGame_Rect.x = 590;
    resumeGame_Rect.y = 400;
    resumeGame_Rect.w = 50;
    resumeGame_Rect.h = 25;

    SDL_Rect settings_Rect;
    settings_Rect.x = 590;
    settings_Rect.y = 450;
    settings_Rect.w = 50;
    settings_Rect.h = 25;
    
    SDL_Rect exit_Rect;
    exit_Rect.x = 590;
    exit_Rect.y = 500;
    exit_Rect.w = 50;
    exit_Rect.h = 25;

    while(!resume && !exit)
    {
        SDL_GetMouseState(&Mx, &My);

        while(SDL_PollEvent(event))
        {
            if(event->type == SDL_QUIT)
            {
                exit = true;
                break;
            }
            if(event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
            {
                if(Mx >= resumeGame_Rect.x && Mx <= resumeGame_Rect.x + resumeGame_Rect.w && My >= resumeGame_Rect.y && My <= resumeGame_Rect.y + resumeGame_Rect.h)
                {
                    resume = true;
                    waiting = true;
                }
                else if(Mx >= settings_Rect.x && Mx <= settings_Rect.x + settings_Rect.w && My >= settings_Rect.y && My <= settings_Rect.y + settings_Rect.h)
                {
                    inGameSettingsMenu(pGame, pRenderer, event, &settings, IMG_LoadTexture(pRenderer, "Resources/bullets.png"));
                }
                else if(Mx >= exit_Rect.x && Mx <= exit_Rect.x + exit_Rect.w && My >= exit_Rect.y && My <= exit_Rect.y + exit_Rect.h)
                {
                    exit = true;
                    closeGame(pGame);
                }
            }
        }
        SDL_RenderClear(pRenderer);
        SDL_RenderCopy(pRenderer, pGame->pbackground, NULL, NULL);
        drawPlatforms(pGame->pPlatforms, pGame->platformCount);
        drawPlayer(pGame->pPlayer);
        for(int i = 0; i < MAX_BULLETS; i++)
        { 
            if(isActive(pGame->pProjectile[i]))
            {
                drawProjectile(pGame->pProjectile[i]);
            }
        }

        SDL_RenderCopy(pRenderer, resumeButton, NULL, &resumeGame_Rect);
        SDL_RenderCopy(pRenderer, settingsButton, NULL, &settings_Rect);
        SDL_RenderCopy(pRenderer, exitGameButton, NULL, &exit_Rect);

        SDL_RenderPresent(pRenderer);

        SDL_Delay(16);
    }

    if(resume)
    {
        *inGameMenu = false;
    }
    else if(exit)
    {
        *inGameMenu = false;
    }

    while (waiting)
    {
        while(SDL_PollEvent(event))
        {
            if(event->type == SDL_MOUSEBUTTONUP)
            {
                waiting = false;
            }
        }
    }
}

