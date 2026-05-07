#ifndef SERVER_NET_H
#define SERVER_NET_H

#include "game_net.h"

typedef struct serverclient ServerClient;
typedef struct servergame ServerGame;

static int initServer(ServerGame *game);
static bool isSameAddress(IPaddress *a, IPaddress *b);
static int findClientId(ServerGame *game, IPaddress *address);
static int addClient(ServerGame *game, IPaddress *address);
static void receiveInputs(ServerGame *game);
static void updateWorld(ServerGame *game, NetTile tileChanges[MAX_TILE_CHANGES], uint8_t *tileChangeCount);
static void prepareClientPacket(ServerGame *game, ServerPacket *serverPacket, int clientId);
static void sendState(ServerGame *game, ServerPacket *serverPacket);
void addChangedTile(NetTile tileChanges[MAX_TILE_CHANGES], uint8_t *tileChangeCount, int x, int y, int newTexture);

#endif