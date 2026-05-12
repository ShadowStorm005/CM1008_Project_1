#ifndef MENU_H
#define MENU_H

#include <SDL.h>

typedef enum {
    MENU_ACTION_NONE,
    MENU_ACTION_PLAY,
    MENU_ACTION_OPTIONS,
    MENU_ACTION_BACK,
    MENU_ACTION_EXIT
} MenuAction;

int menuPointInRect(int x, int y, SDL_Rect rect);

void renderMainMenu(SDL_Renderer *renderer,
                    SDL_Texture *background,
                    SDL_Texture *newGameButton,
                    SDL_Texture *settingsButton,
                    SDL_Texture *exitGameButton);

MenuAction handleMainMenuEvent(SDL_Event *event);

void renderSettingsMenu(SDL_Renderer *renderer,
                        SDL_Texture *background,
                        SDL_Texture *backButton);

MenuAction handleSettingsMenuEvent(SDL_Event *event);

#endif