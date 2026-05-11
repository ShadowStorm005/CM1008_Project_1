#ifndef CLIENT_NET_H
#define CLIENT_NET_H

#include <SDL_net.h>
#include "game_net.h"

typedef struct clientgame ClientGame;

static SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path);
static int initClient(ClientGame *game, const char *serverIp);
static void sendJoin(ClientGame *game);
static uint8_t getInput(const Uint8 *keys);
static void prepareClientPacket(ClientGame *game, ClientPacket *clientPacket);
static void sendInput(ClientGame *game, ClientPacket *clientPacket);
static void updateGameVar(ClientGame *game, ServerPacket *serverPacket, ClientPacket *clientPacket);
static void recieveStatus(ClientGame *game, ServerPacket *serverPacket, ClientPacket *clientPacket);
static void render(ClientGame *game);
static void closeClient(ClientGame *game);

#endif