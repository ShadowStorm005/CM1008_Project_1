#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL_mixer.h>

typedef struct {
    Mix_Chunk *explodenear;
    Mix_Chunk *explodefar;
    Mix_Chunk *tankmoving;
    Mix_Chunk *tankidle;
} Sounds;

int initSound(Sounds *sounds);
void playSound(Mix_Chunk *sound);
int playLoopingSound(Mix_Chunk *sound);
void stopMoveSound(int Channel);
void cleanupSound(Sounds *sounds);

#endif