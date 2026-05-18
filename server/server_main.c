#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_net.h>

#include "player.h"
#include "weapon.h"
#include "map.h"
#include "physics.h"
#include "server_net.h"
#include "game_net.h"
#include "server_creation_functions.h"

struct serverclient {
    bool connected;
    IPaddress ipaddress;
    Player *player;
    uint8_t input;
    int mouseX;
    int mouseY;
};

struct servergame{
    UDPsocket socket;
    UDPpacket *recvPacket;
    UDPpacket *sendPacket;
    ServerClient clients[MAX_PLAYERS];
    Map *map;
    Projectile *projectiles[MAX_BULLETS];
};

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    ServerGame game;
    ServerPacket serverPacket;
    if (!initServer(&game)) return 1;

    int running = 1;
    while (running) {
        Uint32 frameStart = SDL_GetTicks();
        memset(&serverPacket, 0, sizeof(serverPacket));
        receiveInputs(&game);
        if(connectedClientCount(&game) >=2){
            updateWorld(&game, &serverPacket);
        }
        sendStatus(&game, &serverPacket);

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

    game->map = createServerMap(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!game->map) return 0;

    for (int i = 0; i < MAX_BULLETS; i++) {
        game->projectiles[i] = createServerProjectile();
        if (!game->projectiles[i]) return 0;
    }

    //rememberMap(game);
    printf("Server running on UDP port %d\n", SERVER_PORT);
    return 1;
}

static bool isSameAddress(IPaddress *a, IPaddress *b)
{
    return a->host == b->host && a->port == b->port;
}

static int findClientId(ServerGame *game, IPaddress *ipaddress)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (game->clients[i].connected && isSameAddress(&game->clients[i].ipaddress, ipaddress)) return i;
    }
    return -1;
}

static int addClient(ServerGame *game, IPaddress *address)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->clients[i].connected) {
            game->clients[i].connected = 1;
            game->clients[i].ipaddress = *address;
            game->clients[i].input = INPUT_NONE;
            game->clients[i].mouseX = 0;
            game->clients[i].mouseY = 0;

            float spawnX = WINDOW_WIDTH / 2.0f + (float)(i * 80);
            float spawnY = WINDOW_HEIGHT / 2.0f;
            game->clients[i].player = createServerPlayer(spawnX, spawnY, WINDOW_WIDTH, WINDOW_HEIGHT);
            printf("Client %d joined, ipaddress: %d\n", i, game->clients[i].ipaddress);
            return i;
        }
    }
    return -1;
}

static void receiveInputs(ServerGame *game)
{
    while (SDLNet_UDP_Recv(game->socket, game->recvPacket)) {
        ClientPacket clientPacket;
        memcpy(&clientPacket, game->recvPacket->data, sizeof(clientPacket));

        int id = findClientId(game, &game->recvPacket->address);
        if (clientPacket.packetType == CLIENT_JOIN_PACKET) {
            if (id < 0) {
                addClient(game, &game->recvPacket->address);
            }
            continue;
        }

        if (id < 0) id = addClient(game, &game->recvPacket->address);
        if (id < 0) continue;

        game->clients[id].input = clientPacket.input;
        game->clients[id].mouseX = clientPacket.mouseX;
        game->clients[id].mouseY = clientPacket.mouseY;
    }
}

static int connectedClientCount(ServerGame *game)
{
    int count = 0;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (game->clients[i].connected) {
            count++;
        }
    }

    return count;
}

static void handleInput(ServerGame *game, ServerClient *client)
{
    if (client->input & INPUT_LEFT) moveLeft(client->player);
    if (client->input & INPUT_RIGHT) moveRight(client->player);
    if (client->input & INPUT_JUMP) jump(client->player);
    
    steerCanon(client->player, client->mouseX, client->mouseY);
    if ((client->input & INPUT_SHOOT) && canShoot(client->player)) {
        shoot(game->projectiles, getBulletSize(client->player), getBulletSpeed(client->player), getCanonX(client->player), getCanonY(client->player), getAngle(client->player), getCanonMode(client->player));
        setTriggerState(client->player, 0);
    }
    else if (!(client->input & INPUT_SHOOT) && !canShoot(client->player)){
        setTriggerState(client->player, 1);
    }
}

static void updateWorld(ServerGame *game, ServerPacket *serverPacket)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        ServerClient *client = &game->clients[i];
        if (!client->connected || !client->player) continue;
        handleInput(game, client);
        updatePlayer(client->player, game->map, client->mouseX, client->mouseY);
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (isActive(game->projectiles[i])) 
            updateProjectile(game->projectiles[i], 
                            game->map, 
                            serverPacket->tileChanges, 
                            &serverPacket->tileChangeCount);
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        ServerClient *client = &game->clients[i];
        if (!client->connected || !client->player) continue;
        for (int j = 0; j < MAX_BULLETS; j++) {
            if (!isActive(game->projectiles[i])) continue;
            checkBulletPlayerCollision(game->projectiles[j], client->player);
        }
    }
}

static void prepareClientPacket(ServerGame *game, ServerPacket *serverPacket, int clientId)
{
    int connectedCount = connectedClientCount(game);

    serverPacket->playerId = (uint8_t)clientId;

    if (connectedCount < 2) {
        serverPacket->serverState = SERVER_MENU_STATE;
        serverPacket->clientState = CLIENT_LOBBY_STATE;
    }
    else {
        serverPacket->serverState = SERVER_RUN_STATE;
        serverPacket->clientState = CLIENT_PLAYING_STATE;
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->clients[i].connected || !game->clients[i].player)
        {
            continue;
        }
        
        serverPacket->players[i].x = getPlayerX(game->clients[i].player);
        serverPacket->players[i].y = getPlayerY(game->clients[i].player);
        serverPacket->players[i].mouseX = game->clients[i].mouseX;
        serverPacket->players[i].mouseY = game->clients[i].mouseY;
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!isActive(game->projectiles[i]))
        {
            continue;
        }

        serverPacket->projectiles[i].x = getBulletX(game->projectiles[i]);
        serverPacket->projectiles[i].y = getBulletY(game->projectiles[i]);
        serverPacket->projectiles[i].angle = getBulletAngle(game->projectiles[i]);
    }
}

static void sendStatus(ServerGame *game, ServerPacket *serverPacket)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->clients[i].connected)
        {
            continue;
        }
        
        prepareClientPacket(game, serverPacket, i);
        memcpy(game->sendPacket->data, serverPacket, sizeof(*serverPacket));
        game->sendPacket->len = sizeof(*serverPacket);
        game->sendPacket->address = game->clients[i].ipaddress;
        SDLNet_UDP_Send(game->socket, -1, game->sendPacket);
    }
}

static void closeServer(ServerGame *game)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (game->clients[i].player) destroyPlayer(game->clients[i].player);
    }
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->projectiles[i]) destroyProjectile(game->projectiles[i]);
    }
    if (game->map) destroyTiles(game->map);
    if (game->recvPacket) SDLNet_FreePacket(game->recvPacket);
    if (game->sendPacket) SDLNet_FreePacket(game->sendPacket);
    if (game->socket) SDLNet_UDP_Close(game->socket);
    SDLNet_Quit();
    SDL_Quit();
}