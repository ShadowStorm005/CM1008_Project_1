#ifndef GAME_NET_H
#define GAME_NET_H

#include <stdint.h>

#define SERVER_PORT 1234
#define DEFAULT_SERVER_IP "192.168.1.117"

#define MAX_PLAYERS 2
#define MAX_TILE_CHANGES 128
#define UNKNOWN_PLAYER 255
#define FPS 60
#define FRAME_DELAY (1000 / FPS)

#define IP_TEXT_MAX 64
#define JOINS_SEND_INTERVAL_MS 500

typedef enum {
    CLIENT_JOIN_PACKET,
    CLIENT_IDLE_PACKET,
    CLIENT_INPUT_PACKET
} ClientPacketTypes;

typedef enum {
    SKIN_SWEDEN,
    SKIN_DENMARK,
    SKIN_DEUTSCH,
    SKIN_RUSSIA
} TankSkins;

typedef enum {
    INPUT_NONE = 0,
    INPUT_LEFT = 1,
    INPUT_RIGHT = 2,
    INPUT_JUMP = 4,
    INPUT_SHOOT = 8,
    INPUT_1 = 16,
    INPUT_2 = 32
} InputType;

typedef enum {
    CLIENT_MAIN_MENU_STATE,
    CLIENT_CONNECT_STATE,
    CLIENT_OPTIONS_STATE,
    CLIENT_LOBBY_STATE,
    CLIENT_PLAYING_STATE,
    CLIENT_INGAME_MENU_STATE,
    CLIENT_DEAD_STATE,
    CLIENT_QUIT_STATE
} ClientState;

typedef struct {
    int32_t mouseX;
    int32_t mouseY;
    ClientPacketTypes packetType;
    ClientState clientState;
    uint8_t input;
    uint8_t playerId;
    uint8_t tankSkin;
} ClientPacket;

typedef enum {
    SERVER_MENU_STATE,
    SERVER_RUN_STATE,
    SERVER_END_STATE
} ServerState;

typedef struct {
    float x;
    float y;
    int32_t mouseX;
    int32_t mouseY;
    uint32_t smokeTimer;
    uint8_t tankSkin;
    uint8_t health;
} NetPlayer;

typedef struct {
    float x;
    float y;
    float angle;
} NetProjectile;

typedef struct {
    int x;
    int y;
    uint32_t explosionTimer;
} NetExplosion;

typedef struct {
    int8_t x;
    int8_t y;
    int8_t selectedTexture;
} NetTile;

typedef struct {
    NetTile tileChanges[MAX_TILE_CHANGES];
    NetProjectile projectiles[MAX_BULLETS];
    NetExplosion explosions[MAX_BULLETS];
    NetPlayer players[MAX_PLAYERS];
    uint32_t serverTime;
    ServerState serverState;
    ClientState clientState;
    uint8_t playerId;
    uint8_t tileChangeCount;
} ServerPacket;

#endif