#ifndef GAME_NET_H
#define GAME_NET_H

#include <stdint.h>

#define SERVER_PORT 1234
#define DEFAULT_SERVER_IP "172.20.10.3"
#define MAX_PLAYERS 4
#define MAX_TILE_CHANGES 128
#define UNKNOWN_PLAYER 255
#define FPS 60
#define FRAME_DELAY (1000 / FPS)

typedef enum {
    CLIENT_JOIN_PACKET,
    CLIENT_INPUT_PACKET
} ClientPacketTypes;

typedef enum {
    NONE,
    LEFT,
    RIGHT,
    JUMP,
    SHOOT
} InputType;

typedef enum {
    CLIENT_WAITING_STATE,
    CLIENT_PLAYING_STATE,
    CLIENT_DEAD_STATE
} ClientState;

typedef struct {
    int32_t mouseX;
    int32_t mouseY;
    ClientPacketTypes packetType;
    InputType input;
    ClientState clientState;
    uint8_t playerId;
} ClientPacket;

typedef enum {
    SERVER_MENU_STATE,
    SERVER_RUN_STATE,
    SERVER_END_STATE
} ServerState;

typedef struct {
    float x;
    float y;
} NetPlayer;

typedef struct {
    float x;
    float y;
    float angle;
} NetProjectile;

typedef struct {
    int8_t x;
    int8_t y;
    int8_t selectedTexture;
} NetTile;

typedef struct {
    NetTile tileChanges[MAX_TILE_CHANGES];
    NetProjectile projectiles[MAX_BULLETS];
    NetPlayer players[MAX_PLAYERS];
    ServerState serverState;
    ClientState clientState;
    uint8_t playerId;
    uint8_t tileChangeCount;
} ServerPacket;

#endif