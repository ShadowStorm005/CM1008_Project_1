#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_net.h>

#include "player.h"
#include "weapon.h"
#include "map.h"
#include "client_net.h"
#include "game_net.h"

struct clientgame{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background;

    UDPsocket socket;
    UDPpacket *sendPacket;
    UDPpacket *recvPacket;
    IPaddress serverAddress;

    Player *players[MAX_PLAYERS];
    Projectile *projectiles[MAX_BULLETS];
    Map *map;

    bool inGameMenu;
    SDL_Texture *resumeButton;
    SDL_Texture *settingsButton;
    SDL_Texture *exitGameButton;
    SDL_Texture *backButton;
    SDL_Texture *newGameButton;

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

    while (game.clientState == CLIENT_MAIN_MENU_STATE) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) game.clientState = CLIENT_QUIT_STATE;
        }
        sendJoin(&game);
        recieveStatus(&game, &serverPacket);
    }

    while (game.clientState != CLIENT_MAIN_MENU_STATE && 
        game.clientState != CLIENT_LOBBY_STATE &&
        game.clientState != CLIENT_QUIT_STATE &&
        game.serverState == SERVER_RUN_STATE) {
        Uint32 frameStart = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) game.clientState = CLIENT_QUIT_STATE;
        }

        if (game.playerId == UNKNOWN_PLAYER) {
            sendJoin(&game);
        } else {
            sendInput(&game);
        }

        recieveStatus(&game, &serverPacket);
        render(&game);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) SDL_Delay(FRAME_DELAY - frameTime);
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
    game->serverState = CLIENT_MAIN_MENU_STATE;

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

    game->window = SDL_CreateWindow("Tank Turtles Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!game->window) return 0;

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (!game->renderer) return 0;

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
        float spawnX = WINDOW_WIDTH / 2.0f + (float)(i * 80);
        float spawnY = WINDOW_HEIGHT / 2.0f;
        game->players[i] = createPlayer(spawnX, spawnY, game->renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
        if (!game->players[i]) return 0;
    }

    /*game->resumeButton = loadTexture(game->renderer, "Resources/firsttank.png");
    if (!game->resumeButton) return 0;*/

    game->resumeButton = IMG_LoadTexture(game->renderer, "Resources/firsttank.png");
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

    SDL_Texture* resumeGameButton = IMG_LoadTexture(game->renderer, "Resources/firsttank.png");
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

    if (SDLNet_ResolveHost(&game->serverAddress, serverIp, SERVER_PORT) != 0) {
        printf("Could not resolve server %s: %s\n", serverIp, SDLNet_GetError());
        return 0;
    }

    game->sendPacket = SDLNet_AllocPacket(sizeof(ClientPacket));
    game->recvPacket = SDLNet_AllocPacket(sizeof(ServerPacket));
    if (!game->sendPacket || !game->recvPacket) return 0;

    return 1;
}

static void sendJoin(ClientGame *game)
{
    ClientPacket joinPacket;
    memset(&joinPacket, 0, sizeof(joinPacket));

    joinPacket.packetType = CLIENT_JOIN_PACKET;
    joinPacket.playerId = UNKNOWN_PLAYER;

    memcpy(game->sendPacket->data, &joinPacket, sizeof(joinPacket));
    game->sendPacket->len = sizeof(joinPacket);
    game->sendPacket->address = game->serverAddress;

    SDLNet_UDP_Send(game->socket, -1, game->sendPacket);
}

static void sendInput(ClientGame *game)
{

}

static void updateGameVar(ClientGame *game, ServerPacket *serverPacket)
{
    game->playerId = serverPacket->playerId;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->players[i]) continue;
        setPlayerCord(game->players[i], (int)serverPacket->players[i].x, (int)serverPacket->players[i].y);
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
        if (!change->selectedTexture) inactivateTile(game->map, change->x, change->y);
    }
}

static void recieveStatus(ClientGame *game, ServerPacket *serverPacket)
{
    while (SDLNet_UDP_Recv(game->socket, game->recvPacket)) {
        memset(&serverPacket, 0, sizeof(serverPacket));
        memcpy(&serverPacket, game->recvPacket->data, sizeof(serverPacket));

        switch (serverPacket->clientState) {
            case CLIENT_LOBBY_STATE:
                break;
            case CLIENT_PLAYING_STATE:
                updateGameVar(game, serverPacket);
                break;
            case CLIENT_INGAME_MENU_STATE:
                break;
            case CLIENT_DEAD_STATE:
                break;
            case CLIENT_QUIT_STATE:
                break;
        }
        game->clientState = serverPacket->clientState;
        game->serverState = serverPacket->serverState;
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

    SDLNet_Quit();
    IMG_Quit();
    SDL_Quit();
}