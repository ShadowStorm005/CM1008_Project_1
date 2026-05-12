#include "menu.h"
#include "map.h"

#define MENU_BUTTON_X 480
#define MENU_BUTTON_W 300
#define MENU_BUTTON_H 100

static SDL_Rect backgroundRect(void)
{
    SDL_Rect rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    return rect;
}

static SDL_Rect newGameRect(void)
{
    SDL_Rect rect = {MENU_BUTTON_X, 250, MENU_BUTTON_W, MENU_BUTTON_H};
    return rect;
}

static SDL_Rect settingsRect(void)
{
    SDL_Rect rect = {MENU_BUTTON_X, 400, MENU_BUTTON_W, MENU_BUTTON_H};
    return rect;
}

static SDL_Rect exitRect(void)
{
    SDL_Rect rect = {MENU_BUTTON_X, 550, MENU_BUTTON_W, MENU_BUTTON_H};
    return rect;
}

static SDL_Rect backRect(void)
{
    SDL_Rect rect = {MENU_BUTTON_X, 550, MENU_BUTTON_W, MENU_BUTTON_H};
    return rect;
}

int menuPointInRect(int x, int y, SDL_Rect rect)
{
    return x >= rect.x && x <= rect.x + rect.w &&
           y >= rect.y && y <= rect.y + rect.h;
}

void renderMainMenu(SDL_Renderer *renderer,
                    SDL_Texture *background,
                    SDL_Texture *newGameButton,
                    SDL_Texture *settingsButton,
                    SDL_Texture *exitGameButton)
{
    SDL_Rect bg = backgroundRect();
    SDL_Rect newGame = newGameRect();
    SDL_Rect settings = settingsRect();
    SDL_Rect exitGame = exitRect();

    SDL_RenderClear(renderer);

    if (background) SDL_RenderCopy(renderer, background, NULL, &bg);
    if (newGameButton) SDL_RenderCopy(renderer, newGameButton, NULL, &newGame);
    if (settingsButton) SDL_RenderCopy(renderer, settingsButton, NULL, &settings);
    if (exitGameButton) SDL_RenderCopy(renderer, exitGameButton, NULL, &exitGame);

    SDL_RenderPresent(renderer);
}

MenuAction handleMainMenuEvent(SDL_Event *event)
{
    if (event->type == SDL_QUIT) {
        return MENU_ACTION_EXIT;
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        int mx = event->button.x;
        int my = event->button.y;

        if (menuPointInRect(mx, my, newGameRect())) return MENU_ACTION_PLAY;
        if (menuPointInRect(mx, my, settingsRect())) return MENU_ACTION_OPTIONS;
        if (menuPointInRect(mx, my, exitRect())) return MENU_ACTION_EXIT;
    }

    return MENU_ACTION_NONE;
}

void renderSettingsMenu(SDL_Renderer *renderer,
                        SDL_Texture *background,
                        SDL_Texture *backButton)
{
    SDL_Rect bg = backgroundRect();
    SDL_Rect back = backRect();

    SDL_RenderClear(renderer);

    if (background) SDL_RenderCopy(renderer, background, NULL, &bg);
    if (backButton) SDL_RenderCopy(renderer, backButton, NULL, &back);

    SDL_RenderPresent(renderer);
}

MenuAction handleSettingsMenuEvent(SDL_Event *event)
{
    if (event->type == SDL_QUIT) {
        return MENU_ACTION_EXIT;
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (menuPointInRect(event->button.x, event->button.y, backRect())) {
            return MENU_ACTION_BACK;
        }
    }

    return MENU_ACTION_NONE;
}