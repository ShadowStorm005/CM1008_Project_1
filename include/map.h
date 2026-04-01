#ifndef map_h
#define map_h

typedef struct platformImage PlatformImage;
typedef struct platform Platform;

Platform *createPlatform(SDL_Renderer *pRenderer, int window_width, int window_height);
void drawPlatform(Platform *pPlatform);
void destroyPlatform(Platform *pflatform);

#endif