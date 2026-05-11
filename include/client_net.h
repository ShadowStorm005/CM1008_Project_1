#ifndef CLIENT_NET_H
#define CLIENT_NET_H

#include <SDL_net.h>
#include "game_net.h"

typedef struct clientgame ClientGame;

static SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path);
static int initClient(ClientGame *game, const char *serverIp);
static void sendJoin(ClientGame *game);
static void sendInput(ClientGame *game);
static void updateGameVar(ClientGame *game, ServerPacket *serverPacket);
static void recieveStatus(ClientGame *game, ServerPacket *serverPacket);
static void render(ClientGame *game);
static void closeClient(ClientGame *game);
static int connectToServer(ClientGame *game, const char *serverIp);
static void renderConnectMenu(ClientGame *game);
static void renderLobby(ClientGame *game);
static void drawText(ClientGame *game, const char *text, int x, int y, SDL_Color color);

#endif