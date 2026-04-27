#ifndef SOUND_H
#define SOUND_H
#define MOVE_CHANNEL 0
#define IDLE_CHANNEL 1

#include <SDL2/SDL_mixer.h>

typedef struct sounds Sounds;

Sounds *createSound();
void destroySound(Sounds *sounds);
int initSound(Sounds *sounds);
void startIdleSound(Sounds *sounds);
void updateMovementSound(Sounds *sounds, int isMoving);
void playFireSound(Sounds *sounds);                          
void playExplosionSound(Sounds *sounds);
void cleanupSound(Sounds *sounds);
void playMoveSound(Mix_Chunk *sound);

#endif