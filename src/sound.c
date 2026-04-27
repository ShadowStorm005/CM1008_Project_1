#include "sound.h"
#include <stdio.h>

int initSound(Sounds *sounds)
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)<0)
    {
        printf("Audio error %s", Mix_GetError());
        return 0;
    }

    sounds->explodenear = Mix_LoadWAV("Audio/explodenear.wav");
    sounds->explodefar  = Mix_LoadWAV("Audio/explodefar.wav");
    sounds->tankmoving  = Mix_LoadWAV("Audio/tankmoving.wav");
    sounds->tankidle    = Mix_LoadWAV("Audio/tankidle.wav");

    if (!sounds->explodenear) printf("Missing: explodenear.wav\n");
    if (!sounds->explodefar)  printf("Missing: explodefar.wav\n");
    if (!sounds->tankmoving)  printf("Missing: tankmoving.wav\n");
    if (!sounds->tankidle)    printf("Missing: tankidle.wav\n");

    Mix_VolumeChunk(sounds->explodenear, 80);  
    Mix_VolumeChunk(sounds->explodefar,  30);   
    Mix_VolumeChunk(sounds->tankmoving,  30);   
    Mix_VolumeChunk(sounds->tankidle,  40); 

    return 1;
}

void playSound(Mix_Chunk *sound)
{
    if (sound) Mix_PlayChannel(-1, sound, 0);
}

void playMoveSound(Mix_Chunk *sound)
{
    // Only play if channel 0 is not already playing
    if (sound && !Mix_Playing(MOVE_CHANNEL))
        Mix_PlayChannel(MOVE_CHANNEL, sound, 0);
}

int playLoopingSound(Mix_Chunk *sound)
{
    if (sound) return Mix_PlayChannel(-1, sound, -1);
    return -1;
}

void stopMoveSound(int channel)
{
    if (channel >= 0) Mix_HaltChannel(channel);
}

void cleanupSound(Sounds *sounds)
{
    Mix_FreeChunk(sounds->explodenear);
    Mix_FreeChunk(sounds->explodefar);
    Mix_FreeChunk(sounds->tankmoving);
    Mix_FreeChunk(sounds->tankidle);
    Mix_CloseAudio();
}