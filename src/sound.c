#include "sound.h"
#include <stdio.h>

int initSounds(Sounds *sounds)
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

    return 1;
}