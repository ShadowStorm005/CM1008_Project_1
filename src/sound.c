#include "sound.h"
#include <stdio.h>

struct sounds
{
    Mix_Chunk *tankidle;
    Mix_Chunk *tankmoving;
    Mix_Chunk *tankfire;
    Mix_Chunk *explodefar;
};


int initSound(Sounds *sounds)
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)<0)
    {
        printf("Audio error %s", Mix_GetError());
        return 0;
    }

    sounds->tankidle    = Mix_LoadWAV("Audio/tankidle.wav");
    sounds->tankmoving  = Mix_LoadWAV("Audio/tankmoving.wav");
    sounds->tankfire    = Mix_LoadWAV("Audio/tankfire.wav");   
    sounds->explodefar  = Mix_LoadWAV("Audio/explodefar.wav");

    if (!sounds->tankidle)   printf("Missing: tankidle.wav\n");
    if (!sounds->tankmoving) printf("Missing: tankmoving.wav\n");
    if (!sounds->tankfire)   printf("Missing: tankfire.wav\n");   
    if (!sounds->explodefar) printf("Missing: explodefar.wav\n");

    Mix_VolumeChunk(sounds->tankfire, 80);  
    Mix_VolumeChunk(sounds->explodefar,  40);   
    Mix_VolumeChunk(sounds->tankmoving,  30);   
    Mix_VolumeChunk(sounds->tankidle,  40); 

    return 1;
}

void startIdleSound(Sounds *sounds)
{
    if (!Mix_Playing(IDLE_CHANNEL))
        Mix_PlayChannel(IDLE_CHANNEL, sounds->tankidle, -1);
}

void updateMovementSound(Sounds *sounds, int isMoving)
{
    if (isMoving)
    {
        Mix_HaltChannel(IDLE_CHANNEL);
        if (!Mix_Playing(MOVE_CHANNEL))
            Mix_PlayChannel(MOVE_CHANNEL, sounds->tankmoving, -1);  
    }
    else
    {
        Mix_HaltChannel(MOVE_CHANNEL);
        if (!Mix_Playing(IDLE_CHANNEL))
            Mix_PlayChannel(IDLE_CHANNEL, sounds->tankidle, -1);
    }
}

void playFireSound(Sounds *sounds)
{
    if (sounds->tankfire)
        Mix_PlayChannel(-1, sounds->tankfire, 0);
}

void playExplosionSound(Sounds *sounds)
{
    if (sounds->explodefar)
        Mix_PlayChannel(-1, sounds->explodefar, 0);
}

void cleanupSound(Sounds *sounds)
{
    Mix_FreeChunk(sounds->tankidle);
    Mix_FreeChunk(sounds->tankmoving);
    Mix_FreeChunk(sounds->tankfire);      
    Mix_FreeChunk(sounds->explodefar);
    // REMOVE explodenear free
    Mix_CloseAudio();
}
