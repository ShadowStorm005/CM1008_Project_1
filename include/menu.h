#ifndef MENU_H
#define MENU_H

#include <SDL.h>

typedef enum {
    MENU_ACTION_NONE,
    MENU_ACTION_PLAY,
    MENU_ACTION_OPTIONS,
    MENU_ACTION_BACK,
    MENU_ACTION_EXIT,
    MENU_ACTION_SELECT_SWEDEN,
    MENU_ACTION_SELECT_GERMANY,
    MENU_ACTION_SELECT_RUSSIA,
    MENU_ACTION_SELECT_DENMARK
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
                        SDL_Texture *backButton,
                        SDL_Texture *swedenSkinButton,
                        SDL_Texture *germanySkinButton,
                        SDL_Texture *russiaSkinButton,
                        SDL_Texture *denmarkSkinButton);

MenuAction handleSettingsMenuEvent(SDL_Event *event);

#endif