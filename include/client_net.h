#ifndef CLIENT_NET_H
#define CLIENT_NET_H

#include "game_net.h"

typedef struct clientGame ClientGame;

static SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path);
static int initClient(ClientGame *game, const char *serverIp);
static void sendJoin(ClientGame *game);
static void sendInput(ClientGame *game);
static void updateGameVar(ClientGame *game, ServerPacket *serverPacket);
static void receiveStatus(ClientGame *game, ServerPacket *serverPacket);
static void render(ClientGame *game);
static void closeClient(ClientGame *game);

#endif