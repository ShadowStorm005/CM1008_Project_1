#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_net.h>
#include <SDL_ttf.h>

#include "player.h"
#include "weapon.h"
#include "map.h"
#include "client_net.h"
#include "game_net.h"
#include "menu.h"

struct clientgame{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;
    TTF_Font *font;

    UDPsocket socket;
    UDPpacket *sendPacket;
    UDPpacket *recvPacket;
    IPaddress serverAddress;
    bool hasServerAddress;
    Uint32 lastJoinSendTime;

    Player *players[MAX_PLAYERS];
    Projectile *projectiles[MAX_BULLETS];
    Map *map;

    bool inGameMenu;
    SDL_Texture *resumeButton;
    SDL_Texture *settingsButton;
    SDL_Texture *exitGameButton;
    SDL_Texture *backButton;
    SDL_Texture *newGameButton;
    SDL_Texture *winnerTexture;
    SDL_Texture *loserTexture;

    char serverIpText[IP_TEXT_MAX];

    uint8_t playerId;
    ClientState clientState;
    ServerState serverState;
};

int main(int argc, char **argv)
{
    const char *serverIp = DEFAULT_SERVER_IP;
    if (argc >= 2) serverIp = argv[1];

    ClientGame game;
    ClientPacket clientPacket;
    ServerPacket serverPacket;

    if (!initClient(&game, serverIp)) {
        closeClient(&game);
        return 1;
    }

    int running = 1;

    while (running) {
        Uint32 frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                game.clientState = CLIENT_QUIT_STATE;
                running = 0;
            }

            else if (game.clientState == CLIENT_MAIN_MENU_STATE) {
                MenuAction action = handleMainMenuEvent(&event);

                if (action == MENU_ACTION_PLAY) {
                    game.clientState = CLIENT_CONNECT_STATE;
                    SDL_StartTextInput();
                }
                else if (action == MENU_ACTION_OPTIONS) {
                    game.clientState = CLIENT_OPTIONS_STATE;
                }
                else if (action == MENU_ACTION_EXIT) {
                    game.clientState = CLIENT_QUIT_STATE;
                    running = 0;
                }
            }

            else if (game.clientState == CLIENT_OPTIONS_STATE) {
                MenuAction action = handleSettingsMenuEvent(&event);

                if (action == MENU_ACTION_BACK) {
                    game.clientState = CLIENT_MAIN_MENU_STATE;
                }
                else if (action == MENU_ACTION_EXIT) {
                    game.clientState = CLIENT_QUIT_STATE;
                    running = 0;
                }
            }

            else if (game.clientState == CLIENT_CONNECT_STATE) {
                if (event.type == SDL_TEXTINPUT) {
                    if (strlen(game.serverIpText) + strlen(event.text.text) < IP_TEXT_MAX - 1) {
                        strcat(game.serverIpText, event.text.text);
                    }
                }

                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(game.serverIpText) > 0) {
                        game.serverIpText[strlen(game.serverIpText) - 1] = '\0';
                    }
                    else if (event.key.keysym.sym == SDLK_RETURN) {
                        if (connectToServer(&game, game.serverIpText)) {
                            SDL_StopTextInput();
                            game.clientState = CLIENT_LOBBY_STATE;
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        SDL_StopTextInput();
                        game.clientState = CLIENT_MAIN_MENU_STATE;
                    }
                }

                else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    SDL_Rect connectButton = { WINDOW_WIDTH / 2 - 150, 450, 300, 100 };
                    SDL_Rect backButton = { WINDOW_WIDTH / 2 - 150, 570, 300, 100 };

                    if (menuPointInRect(event.button.x, event.button.y, connectButton)) {
                        if (connectToServer(&game, game.serverIpText)) {
                            SDL_StopTextInput();
                            game.clientState = CLIENT_LOBBY_STATE;
                        }
                    }
                    else if (menuPointInRect(event.button.x, event.button.y, backButton)) {
                        SDL_StopTextInput();
                        game.clientState = CLIENT_MAIN_MENU_STATE;
                    }
                }
                
            }
            else if (game.clientState == CLIENT_END_STATE) {
                if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    SDL_Rect playAgainButton = { WINDOW_WIDTH / 2 - 170, 610, 340, 80 };
                    SDL_Rect disconnectButton = { WINDOW_WIDTH / 2 - 170, 710, 340, 80 };

                    if (menuPointInRect(event.button.x, event.button.y, playAgainButton)) {
                        sendReplay(&game);
                        game.clientState = CLIENT_LOBBY_STATE;
                    }
                    else if (menuPointInRect(event.button.x, event.button.y, disconnectButton)) {
                        sendDisconnect(&game);
                        game.clientState = CLIENT_QUIT_STATE;
                        running = 0;
                    }
                }
            }
            else if (game.clientState == CLIENT_LOBBY_STATE) {
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    game.clientState = CLIENT_MAIN_MENU_STATE;
                }
            }
            else if (game.clientState == CLIENT_PLAYING_STATE) {
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    game.clientState = CLIENT_INGAME_MENU_STATE;
                    game.inGameMenu = true;
                }
            }
        }

        recieveStatus(&game, &serverPacket, &clientPacket);

        if (game.clientState == CLIENT_MAIN_MENU_STATE) {
            renderMainMenu(game.renderer,
                           game.background,
                           game.newGameButton,
                           game.settingsButton,
                           game.exitGameButton);
        }

        else if (game.clientState == CLIENT_OPTIONS_STATE) {
            renderSettingsMenu(game.renderer, game.background, game.backButton);
        }

        else if (game.clientState == CLIENT_CONNECT_STATE) {
            renderConnectMenu(&game);
        }
        else if (game.clientState == CLIENT_LOBBY_STATE) {
            if (SDL_GetTicks() - game.lastJoinSendTime >= JOINS_SEND_INTERVAL_MS) {
                sendJoin(&game);
                game.lastJoinSendTime = SDL_GetTicks();
            }

            recieveStatus(&game, &serverPacket, &clientPacket);
            renderLobby(&game);
        }
        else if (game.clientState == CLIENT_PLAYING_STATE) {
            prepareClientPacket(&game, &clientPacket);
            sendInput(&game, &clientPacket);
            recieveStatus(&game, &serverPacket, &clientPacket);
            render(&game);
        }
        else if (game.clientState == CLIENT_DEAD_STATE) {
            recieveStatus(&game, &serverPacket, &clientPacket);
            render(&game);
        }
        else if (game.clientState == CLIENT_END_STATE) {
            recieveStatus(&game, &serverPacket, &clientPacket);
            renderEndScreen(&game, &serverPacket);
        }
        else if (game.clientState == CLIENT_QUIT_STATE) {
            running = 0;
        }

        recieveStatus(&game, &serverPacket, &clientPacket);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    closeClient(&game);
    return 0;
}

static SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        printf("Could not load %s: %s\n", path, IMG_GetError());
        free(texture);
        return NULL;
    }
    return texture;
}

static int initClient(ClientGame *game, const char *serverIp)
{
    memset(game, 0, sizeof(*game));
    game->playerId = UNKNOWN_PLAYER;
    game->clientState = CLIENT_MAIN_MENU_STATE;
    game->serverState = SERVER_MENU_STATE;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init failed: %s\n", IMG_GetError());
        return 0;
    }
    if (SDLNet_Init() != 0) {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        return 0;
    }

    if (TTF_Init() != 0) {
    printf("TTF_Init failed: %s\n", TTF_GetError());
    return 0;
    }

    game->window = SDL_CreateWindow("Tank Turtles Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!game->window) return 0;

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) return 0;

    game->font = TTF_OpenFont("Resources/comic.ttf", 28);
    if (!game->font) {
        printf("Could not load font: %s\n", TTF_GetError());
    }

    SDL_Surface *pBackground = IMG_Load("Resources/skybackground.png");
    if (!pBackground) {
        printf("Error loading skybackground.png: %s\n", IMG_GetError());
        free(pBackground);
        return 0;
    }
    game->background = SDL_CreateTextureFromSurface(game->renderer, pBackground);
    free(pBackground);

    game->map = createMap(game->renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!game->map) return 0;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        float spawnX[4] = {
            180.0f,
            1010.0f,
            180.0f,
            1010.0f
        };

        float spawnY[4] = {
            735.0f,
            360.0f,
            210.0f,
            770.0f
        };

        game->players[i] = createPlayer(spawnX[i], spawnY[i], game->renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
        if (!game->players[i]) return 0;
    }

    /*game->resumeButton = loadTexture(game->renderer, "Resources/firsttank.png");
    if (!game->resumeButton) return 0;*/

    game->resumeButton = IMG_LoadTexture(game->renderer, "Resources/Sprite-backButton.png");
    if (!game->resumeButton) {
        printf("Error loading resumeButton.png: %s\n", IMG_GetError());
        free(game->resumeButton);
        return 0;
    }

    game->newGameButton = IMG_LoadTexture(game->renderer, "Resources/Sprite-startButton.png");
    if (!game->newGameButton) {
        printf("Error loading newGame.png: %s\n", IMG_GetError());
        free(game->newGameButton);
        return 0;
    }

    game->settingsButton = IMG_LoadTexture(game->renderer, "Resources/Sprite-settingsButton.png");
    if (!game->settingsButton) {
        printf("Error loading settings.png: %s\n", IMG_GetError());
        free(game->settingsButton);
        return 0;
    }

    game->exitGameButton = IMG_LoadTexture(game->renderer, "Resources/Sprite-exitButton.png");
    if (!game->exitGameButton) {
        printf("Error loading exitGame.png: %s\n", IMG_GetError());
        free(game->exitGameButton);
        return 0;
    }

    game->backButton = IMG_LoadTexture(game->renderer, "Resources/Sprite-backButton.png");
    if (!game->backButton) {
        printf("Error loading backButton.png: %s\n", IMG_GetError());
        free(game->backButton);
        return 0;
    }

    game->winnerTexture = IMG_LoadTexture(game->renderer, "Resources/Sprite-winnerBanner.png");
    if (!game->winnerTexture) {
        printf("Error loading YouWin.png: %s\n", IMG_GetError());
    }

    game->loserTexture = IMG_LoadTexture(game->renderer, "Resources/Sprite-loseBanner.png");
    if (!game->loserTexture) {
        printf("Error loading YouLose.png: %s\n", IMG_GetError());
    }

    SDL_Texture* resumeGameButton = IMG_LoadTexture(game->renderer, "Resources/Sprite-backButton.png");
    if (!resumeGameButton) {
        printf("Error loading resumeGame.png: %s\n", IMG_GetError());
        free(resumeGameButton);
        return 0;
    }

    for(int i = 0; i < MAX_BULLETS; i++)
    {
        game->projectiles[i] = createProjectile(game->renderer);
        if (!game->projectiles[i])
        {
            printf("Projectile creation failed\n");
            closeClient(game);
            return 0;
        }
    }
    game->socket = SDLNet_UDP_Open(0);
    if (!game->socket) {
        printf("Could not open UDP socket: %s\n", SDLNet_GetError());
        return 0;
    }

    strncpy(game->serverIpText, serverIp, IP_TEXT_MAX - 1);
    game->serverIpText[IP_TEXT_MAX - 1] = '\0';
    game->hasServerAddress = false; 

    if (SDLNet_ResolveHost(&game->serverAddress, serverIp, SERVER_PORT) != 0) {
        printf("Could not resolve server %s: %s\n", serverIp, SDLNet_GetError());
        return 0;
    }

    game->sendPacket = SDLNet_AllocPacket(sizeof(ClientPacket));
    game->recvPacket = SDLNet_AllocPacket(sizeof(ServerPacket));
    if (!game->sendPacket || !game->recvPacket) return 0;
    printf("Init complete\n");
    return 1;
}

static int connectToServer(ClientGame *game, const char *serverIp)
{
    if (!serverIp || strlen(serverIp) == 0) {
        return 0;
    }

    if (SDLNet_ResolveHost(&game->serverAddress, serverIp, SERVER_PORT) != 0) {
        printf("Could not resolve server %s: %s\n", serverIp, SDLNet_GetError());
        return 0;
    }

    game->hasServerAddress = true;
    game->playerId = UNKNOWN_PLAYER;
    game->serverState = SERVER_MENU_STATE;
    game->lastJoinSendTime = 0;

    printf("Trying to connect to server %s:%d\n", serverIp, SERVER_PORT);
    return 1;
}

static void sendJoin(ClientGame *game)
{
    ClientPacket joinPacket;
    memset(&joinPacket, 0, sizeof(joinPacket));

    joinPacket.packetType = CLIENT_JOIN_PACKET;
    joinPacket.playerId = UNKNOWN_PLAYER;
    joinPacket.clientState = CLIENT_LOBBY_STATE;

    memcpy(game->sendPacket->data, &joinPacket, sizeof(joinPacket));
    game->sendPacket->len = sizeof(joinPacket);
    game->sendPacket->address = game->serverAddress;

    // SDLNet_UDP_Send(game->socket, -1, game->sendPacket);

    int sent = SDLNet_UDP_Send(game->socket, -1, game->sendPacket);

    if (!sent) {
        printf("Failed to send join packet: %s\n", SDLNet_GetError());
    } else {
        printf("Join packet sent to server\n");
    }
}

static uint8_t getInput(const Uint8 *keys)
{
    uint8_t buttons = 0;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) buttons |= INPUT_LEFT;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) buttons |= INPUT_RIGHT;
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) buttons |= INPUT_JUMP;
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT) || keys[SDL_SCANCODE_SPACE]) buttons |= INPUT_SHOOT;
    if (keys[SDL_SCANCODE_1]) buttons |= INPUT_1;
    if (keys[SDL_SCANCODE_2]) buttons |= INPUT_2;

    return buttons;
}

static void prepareClientPacket(ClientGame *game, ClientPacket *clientPacket)
{
    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    memset(clientPacket, 0, sizeof(*clientPacket));
    clientPacket->packetType = CLIENT_INPUT_PACKET;
    clientPacket->playerId = game->playerId;
    clientPacket->input = getInput(keys);
    clientPacket->mouseX = mouseX;
    clientPacket->mouseY = mouseY;
    clientPacket->tankSkin = SKIN_DENMARK; // Change Skin Here
}

static void sendInput(ClientGame *game, ClientPacket *clientPacket)
{
    if (!game->hasServerAddress || game->playerId == UNKNOWN_PLAYER) {
        return;
    }

    memcpy(game->sendPacket->data, clientPacket, sizeof(*clientPacket));
    game->sendPacket->len = sizeof(*clientPacket);
    game->sendPacket->address = game->serverAddress;

    SDLNet_UDP_Send(game->socket, -1, game->sendPacket);
}

static void sendReplay(ClientGame *game)
{
    ClientPacket packet;
    memset(&packet, 0, sizeof(packet));

    packet.packetType = CLIENT_REPLAY_PACKET;
    packet.playerId = game->playerId;

    memcpy(game->sendPacket->data, &packet, sizeof(packet));
    game->sendPacket->len = sizeof(packet);
    game->sendPacket->address = game->serverAddress;

    SDLNet_UDP_Send(game->socket, -1, game->sendPacket);
}


static void sendDisconnect(ClientGame *game)
{
    ClientPacket packet;
    memset(&packet, 0, sizeof(packet));

    packet.packetType = CLIENT_DISCONNECT_PACKET;
    packet.playerId = game->playerId;

    memcpy(game->sendPacket->data, &packet, sizeof(packet));
    game->sendPacket->len = sizeof(packet);
    game->sendPacket->address = game->serverAddress;

    SDLNet_UDP_Send(game->socket, -1, game->sendPacket);
}

static void updateGameVar(ClientGame *game, ServerPacket *serverPacket, ClientPacket *clientPacket)
{
    game->playerId = serverPacket->playerId;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->players[i]) continue;
        setPlayerCord(game->players[i], (int)serverPacket->players[i].x, (int)serverPacket->players[i].y);
        steerCanon(game->players[i], serverPacket->players[i].mouseX, serverPacket->players[i].mouseY);
        updatePlayerRects(game->players[i]);
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        setProjectileVar(game->projectiles[i], 1,
                        serverPacket->projectiles[i].x,
                        serverPacket->projectiles[i].y,
                        serverPacket->projectiles[i].angle);
        updateProjectileRect(game->projectiles[i]);
    }
    
    for (int i = 0; i < serverPacket->tileChangeCount && i < MAX_TILE_CHANGES; i++) {
        const NetTile *change = &serverPacket->tileChanges[i];
        setSelectedTexture(game->map, change->x, change->y, change->selectedTexture);
        if (change->selectedTexture == -1) inactivateTile(game->map, change->x, change->y);
    }
}

static void recieveStatus(ClientGame *game, ServerPacket *serverPacket, ClientPacket *clientPacket)
{
    while (SDLNet_UDP_Recv(game->socket, game->recvPacket)) {
        memset(serverPacket, 0, sizeof(*serverPacket));
        memcpy(serverPacket, game->recvPacket->data, sizeof(*serverPacket));

        switch (serverPacket->clientState) {
            case CLIENT_LOBBY_STATE:
                game->clientState = CLIENT_LOBBY_STATE;
                break;
            case CLIENT_PLAYING_STATE:
                updateGameVar(game, serverPacket, clientPacket);
                game->clientState = CLIENT_PLAYING_STATE;
                break;
            case CLIENT_INGAME_MENU_STATE:
                game->clientState = CLIENT_DEAD_STATE;
                break;
            case CLIENT_DEAD_STATE:
                updateGameVar(game, serverPacket, clientPacket);
                game->clientState = CLIENT_DEAD_STATE;
                break;
            case CLIENT_END_STATE:
                updateGameVar(game, serverPacket, clientPacket);
                game->clientState = CLIENT_END_STATE;
                break;
            case CLIENT_QUIT_STATE:
                printf("Ignoring invalid clientState from server: %d\n",
                       serverPacket->clientState);
                break;
        }
        game->serverState = serverPacket->serverState;
        game->playerId = serverPacket->playerId;
        for(int i = 0; i < MAX_PLAYERS; i++)
        {
            changePlayerSkin(game->players[i], serverPacket->players[i].tankSkin);
        }
    }
}

static void render(ClientGame *game)
{
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    SDL_RenderCopy(game->renderer, game->background, NULL, NULL);
    drawTiles(game->map);

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (game->players[i]) {
            drawPlayer(game->players[i]);
            if (i == game->playerId) drawTrajectory(game->players[i], getBulletSpeed(game->players[i]), game->map);
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (isActive(game->projectiles[i])) drawProjectile(game->projectiles[i]);
    }

    SDL_RenderPresent(game->renderer);
}

static void closeClient(ClientGame *game)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (game->players[i]) destroyPlayer(game->players[i]);
    }
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->projectiles[i]) destroyProjectile(game->projectiles[i]);
    }
    if (game->map) destroyTiles(game->map);
    if (game->background) SDL_DestroyTexture(game->background);
    if (game->sendPacket) SDLNet_FreePacket(game->sendPacket);
    if (game->recvPacket) SDLNet_FreePacket(game->recvPacket);
    if (game->socket) SDLNet_UDP_Close(game->socket);
    if (game->renderer) SDL_DestroyRenderer(game->renderer);
    if (game->window) SDL_DestroyWindow(game->window);

    if (game->resumeButton) SDL_DestroyTexture(game->resumeButton);
    if (game->newGameButton) SDL_DestroyTexture(game->newGameButton);
    if (game->settingsButton) SDL_DestroyTexture(game->settingsButton);
    if (game->exitGameButton) SDL_DestroyTexture(game->exitGameButton);
    if (game->backButton) SDL_DestroyTexture(game->backButton);
    if (game->winnerTexture) SDL_DestroyTexture(game->winnerTexture);
    if (game->loserTexture) SDL_DestroyTexture(game->loserTexture);

    SDLNet_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

static void renderConnectMenu(ClientGame *game)
{
    SDL_SetWindowTitle(game->window, "Tank Turtles Client - Connect");

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    if (game->background) {
        SDL_RenderCopy(game->renderer, game->background, NULL, NULL);
    }

    drawText(game, "CONNECT TO SERVER", WINDOW_WIDTH / 2 - 190, 160, (SDL_Color){255, 255, 255, 255});
    drawText(game, "Type the host/server IP address:", WINDOW_WIDTH / 2 - 240, 270, (SDL_Color){255, 255, 255, 255});

    SDL_Rect inputBox = { WINDOW_WIDTH / 2 - 260, 330, 520, 70 };
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(game->renderer, &inputBox);

    drawText(game, game->serverIpText, inputBox.x + 20, inputBox.y + 18, (SDL_Color){255, 255, 255, 255});

    SDL_Rect connectButton = { WINDOW_WIDTH / 2 - 150, 450, 300, 100 };
    SDL_Rect backButton = { WINDOW_WIDTH / 2 - 150, 570, 300, 100 };

    if (game->newGameButton) {
        SDL_RenderCopy(game->renderer, game->newGameButton, NULL, &connectButton);
    }

    if (game->backButton) {
        SDL_RenderCopy(game->renderer, game->backButton, NULL, &backButton);
    }

    drawText(game, "Press Enter to connect. Backspace deletes.", WINDOW_WIDTH / 2 - 260, 700, (SDL_Color){255, 255, 255, 255});

    SDL_RenderPresent(game->renderer);
}

static void renderLobby(ClientGame *game)
{
    SDL_SetWindowTitle(game->window, "Tank Turtles Client - Lobby");

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    if (game->background) {
        SDL_RenderCopy(game->renderer, game->background, NULL, NULL);
    }

    drawText(game, "LOBBY", WINDOW_WIDTH / 2 - 70, 220, (SDL_Color){255, 255, 255, 255});
    drawText(game, "Waiting for another player...", WINDOW_WIDTH / 2 - 230, 330, (SDL_Color){255, 255, 255, 255});
    drawText(game, "The game starts automatically when 2 clients are connected.", WINDOW_WIDTH / 2 - 390, 390, (SDL_Color){255, 255, 255, 255});
    drawText(game, "Press ESC to go back to menu.", WINDOW_WIDTH / 2 - 230, 480, (SDL_Color){255, 255, 255, 255});

    SDL_RenderPresent(game->renderer);
}

static void renderEndScreen(ClientGame *game, ServerPacket *serverPacket)
{
    SDL_SetWindowTitle(game->window, "Tank Turtles Client - Game Over");

    SDL_RenderClear(game->renderer);

    if (game->background) {
        SDL_RenderCopy(game->renderer, game->background, NULL, NULL);
    }

    SDL_Texture *resultTexture;

    if (game->playerId == serverPacket->winnerId) {
        resultTexture = game->winnerTexture;
    }
    else {
        resultTexture = game->loserTexture;
    }

    SDL_Rect resultRect = {
        WINDOW_WIDTH / 2 - 200,
        130,
        400,
        190
    };

    if (resultTexture) {
        SDL_RenderCopy(game->renderer, resultTexture, NULL, &resultRect);
    }

    SDL_Rect playAgainButton = {
        WINDOW_WIDTH / 2 - 170,
        610,
        340,
        80
    };

    SDL_Rect disconnectButton = {
        WINDOW_WIDTH / 2 - 170,
        710,
        340,
        80
    };

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(game->renderer, &playAgainButton);
    SDL_RenderFillRect(game->renderer, &disconnectButton);

    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(game->renderer, &playAgainButton);
    SDL_RenderDrawRect(game->renderer, &disconnectButton);

    drawText(game, "PLAY AGAIN", playAgainButton.x + 80, playAgainButton.y + 22, (SDL_Color){255, 255, 255, 255});
    drawText(game, "DISCONNECT", disconnectButton.x + 65, disconnectButton.y + 22, (SDL_Color){255, 255, 255, 255});

    SDL_RenderPresent(game->renderer);
}


static void drawText(ClientGame *game, const char *text, int x, int y, SDL_Color color)
{
    if (!game->font || !text) return;

    SDL_Surface *surface = TTF_RenderText_Blended(game->font, text, color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dst = { x, y, surface->w, surface->h };
    SDL_RenderCopy(game->renderer, texture, NULL, &dst);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}