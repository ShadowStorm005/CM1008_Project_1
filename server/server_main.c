#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_net.h>

#include "player.h"
#include "weapon.h"
#include "explosions.h"
#include "map.h"
#include "physics.h"
#include "server_net.h"
#include "game_net.h"
#include "server_creation_functions.h"
// 👍👍👍👍👍👍👍👍👍

struct serverclient {
    bool connected;
    IPaddress ipaddress;
    Player *player;
    uint32_t smokeTimer;
    uint8_t input;
    uint8_t tankSkin;
    int mouseX;
    int mouseY;
};

struct servergame {
    UDPsocket socket;
    UDPpacket *recvPacket;
    UDPpacket *sendPacket;
    ServerClient clients[MAX_PLAYERS];
    Map *map;
    Projectile *projectiles[MAX_BULLETS];
    ServerState serverState;
    Explosion *explosions[MAX_BULLETS];
};

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    ServerGame game;
    ServerPacket serverPacket;

    if (!initServer(&game)) {
        return 1;
    }

    int running = 1;

    while (running) {
        Uint32 frameStart = SDL_GetTicks();

        memset(&serverPacket, 0, sizeof(serverPacket));

        receivePacket(&game);

        if (game.serverState == SERVER_MENU_STATE && connectedClientCount(&game) >= MAX_PLAYERS) {
            game.serverState = SERVER_RUN_STATE;
        }

        if (game.serverState == SERVER_RUN_STATE) {
            updateWorld(&game, &serverPacket);

            if (aliveClientCount(&game) <= 1 && connectedClientCount(&game) >= 2) {
                game.serverState = SERVER_END_STATE;
            }
        }

        sendStatus(&game, &serverPacket);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    closeServer(&game);
    return 0;
}

static int initServer(ServerGame *game)
{
    memset(game, 0, sizeof(*game));

    game->serverState = SERVER_MENU_STATE;

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

        game->explosions[i] = createServerExplosion();
        if (!game->explosions[i]) return 0;
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

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->clients[i].connected) {
            game->clients[i].connected = 1;
            game->clients[i].ipaddress = *address;
            game->clients[i].input = INPUT_NONE;
            game->clients[i].mouseX = 0;
            game->clients[i].mouseY = 0;
            game->clients[i].tankSkin = SKIN_SWEDEN;

            game->clients[i].player = createServerPlayer(spawnX[i], spawnY[i], WINDOW_WIDTH, WINDOW_HEIGHT);

            printf("Client %d joined, ipaddress: %d\n", i, game->clients[i].ipaddress.host);
            return i;
        }
    }

    return -1;
}

static void receivePacket(ServerGame *game)
{
    while (SDLNet_UDP_Recv(game->socket, game->recvPacket)) {
        printf("Server received a packet\n");
        ClientPacket clientPacket;
        memcpy(&clientPacket, game->recvPacket->data, sizeof(clientPacket));

        int id = findClientId(game, &game->recvPacket->address);

        if (clientPacket.packetType == CLIENT_DISCONNECT_PACKET) {
            if (id >= 0) {
                game->clients[id].connected = false;
                game->clients[id].input = INPUT_NONE;

                if (game->clients[id].player) {
                    destroyPlayer(game->clients[id].player);
                    game->clients[id].player = NULL;
                }
            }
            continue;
        }
        if (clientPacket.packetType == CLIENT_REPLAY_PACKET) {
            if (game->serverState == SERVER_END_STATE) {
                resetRound(game);
            }
            continue;
        }
        if (clientPacket.packetType == CLIENT_JOIN_PACKET) {
            if (id < 0) {
                id = addClient(game, &game->recvPacket->address);
            }

            if (id >= 0) {
                game->clients[id].tankSkin = clientPacket.tankSkin;
            }
            continue;
        }

        if (id < 0) id = addClient(game, &game->recvPacket->address);
        if (id < 0) continue;

        if (game->serverState != SERVER_RUN_STATE) continue;

        game->clients[id].input = clientPacket.input;
        game->clients[id].mouseX = clientPacket.mouseX;
        game->clients[id].mouseY = clientPacket.mouseY;
        game->clients[id].tankSkin = clientPacket.tankSkin;
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

static int aliveClientCount(ServerGame *game)
{
    int count = 0;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (game->clients[i].connected &&
            game->clients[i].player &&
            getPlayerHealth(game->clients[i].player) > 0) {
            count++;
        }
    }

    return count;
}


static void resetRound(ServerGame *game)
{
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

    if (game->map) {
        destroyTiles(game->map);
    }

    game->map = createServerMap(WINDOW_WIDTH, WINDOW_HEIGHT);

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->projectiles[i]) {
            inactivateBullet(game->projectiles[i]);
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->clients[i].connected) {
            continue;
        }

        if (game->clients[i].player) {
            destroyPlayer(game->clients[i].player);
        }

        game->clients[i].player = createServerPlayer(spawnX[i], spawnY[i], WINDOW_WIDTH, WINDOW_HEIGHT);
        game->clients[i].input = INPUT_NONE;
        game->clients[i].mouseX = 0;
        game->clients[i].mouseY = 0;
    }

    game->serverState = SERVER_MENU_STATE;
}

static int getWinnerId(ServerGame *game)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (game->clients[i].connected &&
            game->clients[i].player &&
            getPlayerHealth(game->clients[i].player) > 0) {
            return i;
        }
    }

    return UNKNOWN_PLAYER;
}
static void handleInput(ServerGame *game, ServerClient *client)
{
    if (getPlayerHealth(client->player) <= 0) {
        client->input = INPUT_NONE;
        return;
    }
    if (client->input & INPUT_LEFT) moveLeft(client->player);
    if (client->input & INPUT_RIGHT) moveRight(client->player);
    if (client->input & INPUT_JUMP) jump(client->player);
    steerCanon(client->player, client->mouseX, client->mouseY);
    if ((client->input & INPUT_SHOOT) && canShoot(client->player)) {
        client->smokeTimer = SDL_GetTicks();
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

        if (getPlayerHealth(client->player) > 0) {
            updatePlayer(client->player, game->map, client->mouseX, client->mouseY);
        }
    }
    int nextExplosion;
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if(!isExplosionActive(game->explosions[i])) nextExplosion = i;
    }

    for (int i = 0; i < MAX_BULLETS; i++) 
    {
        if (isActive(game->projectiles[i]))
            updateProjectile(game->projectiles[i], 
                            game->map,
                            game->explosions[nextExplosion],
                            serverPacket->tileChanges, 
                            &serverPacket->tileChangeCount);
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        ServerClient *client = &game->clients[i];
        if (!client->connected || !client->player) continue;

        for (int j = 0; j < MAX_BULLETS; j++) {
            if (!isActive(game->projectiles[j])) continue;
            checkBulletPlayerCollision(game->projectiles[j], client->player, game->explosions[nextExplosion]);
        }
    }
}

static void prepareClientPacket(ServerGame *game, ServerPacket *serverPacket, int clientId)
{
    int connectedCount = connectedClientCount(game);

    serverPacket->playerId = (uint8_t)clientId;
    serverPacket->serverState = game->serverState;
    serverPacket->winnerId = UNKNOWN_PLAYER;
    
    if (game->serverState == SERVER_END_STATE) {
        serverPacket->clientState = CLIENT_END_STATE;
        serverPacket->winnerId = (uint8_t)getWinnerId(game);
    }
    else if (connectedCount < MAX_PLAYERS) {
        serverPacket->serverState = SERVER_MENU_STATE;
        serverPacket->clientState = CLIENT_LOBBY_STATE;
    }
    else {
        serverPacket->serverState = SERVER_RUN_STATE;
        serverPacket->serverTime = SDL_GetTicks();
        
        if (getPlayerHealth(game->clients[clientId].player) <= 0) {
        serverPacket->clientState = CLIENT_DEAD_STATE;
        }
        else {
            serverPacket->clientState = CLIENT_PLAYING_STATE;
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!game->clients[i].connected || !game->clients[i].player) continue;

        serverPacket->players[i].x = getPlayerX(game->clients[i].player);
        serverPacket->players[i].y = getPlayerY(game->clients[i].player);
        serverPacket->players[i].mouseX = game->clients[i].mouseX;
        serverPacket->players[i].mouseY = game->clients[i].mouseY;
        serverPacket->players[i].tankSkin = game->clients[i].tankSkin;
        int health = getPlayerHealth(game->clients[i].player);

        if (health < 0) health = 0;

        serverPacket->players[i].health = (uint8_t)health;
       
    }

    for (int i = 0; i < MAX_BULLETS; i++) 
    {
        if (!isActive(game->projectiles[i]))
        {
            continue;
        }

        serverPacket->projectiles[i].x = getBulletX(game->projectiles[i]);
        serverPacket->projectiles[i].y = getBulletY(game->projectiles[i]);
        serverPacket->projectiles[i].angle = getBulletAngle(game->projectiles[i]);
    }
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!isExplosionActive(game->explosions[i]))
        {
            continue;
        }
        serverPacket->explosions[i].x = getExplosionCordX(game->explosions[i]);
        serverPacket->explosions[i].y = getExplosionCordY(game->explosions[i]);
        serverPacket->explosions[i].explosionTimer = getStartTime(game->explosions[i]);
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
        if (game->explosions[i])  destroyExplosion(game->explosions[i]);
    }
    if (game->map) destroyTiles(game->map);
    if (game->recvPacket) SDLNet_FreePacket(game->recvPacket);
    if (game->sendPacket) SDLNet_FreePacket(game->sendPacket);
    if (game->socket) SDLNet_UDP_Close(game->socket);
    SDLNet_Quit();
    SDL_Quit();
}