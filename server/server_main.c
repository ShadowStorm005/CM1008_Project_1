#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_net.h>

#include "player.h"
#include "weapon.h"
#include "map.h"
#include "server_net.h"
#include "game_net.h"

typedef struct {
    bool connected;
    IPaddress ipaddress;
    Player *player;
    InputType input;
    int mouseX;
    int mouseY;
} ServerClient;

typedef struct {
    UDPsocket socket;
    UDPpacket *recvPacket;
    UDPpacket *sendPacket;
    ServerClient clients[MAX_PLAYERS];
    Map *map;
    Projectile *projectiles[MAX_BULLETS];
} ServerGame;

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    ServerGame game;
    if (!initServer(&game)) return 1;

    int running = 1;
    while (running) {
        Uint32 frameStart = SDL_GetTicks();

        receiveInputs(&game);
        updateWorld(&game);
        sendState(&game);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) SDL_Delay(FRAME_DELAY - frameTime);
    }

    closeServer(&game);
    return 0;
}

static int initServer(ServerGame *game)
{
    memset(game, 0, sizeof(*game));

    if (SDL_Init(SDL_INIT_TIMER) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }
    if (SDLNet_Init() != 0) {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        return 0;
    }

    game->socket = SDLNet_UDP_Open(SERVER_PORT);
    if (!game->socket) {
        printf("Could not open UDP port %d: %s\n", SERVER_PORT, SDLNet_GetError());
        return 0;
    }

    game->recvPacket = SDLNet_AllocPacket(sizeof(ClientPacket));
    game->sendPacket = SDLNet_AllocPacket(sizeof(ServerPacket));
    if (!game->recvPacket || !game->sendPacket) return 0;

    game->map = createMapHeadless(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!game->map) return 0;

    for (int i = 0; i < MAX_BULLETS; i++) {
        game->projectiles[i] = createProjectileHeadless();
        if (!game->projectiles[i]) return 0;
    }

    rememberMap(game);
    printf("Server running on UDP port %d\n", SERVER_PORT);
    return 1;
}