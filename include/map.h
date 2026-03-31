#ifndef map_h
#define map_h

typedef struct platformImage PlatformImage;
typedef struct platform Platform;

PlatformImage *createPlatformImage(SDL_Renderer *pRenderer);
Platform *createPlatform(PlatformImage *pPlatformImage, int window_width, int window_height);
void destroyPlatform(Platform *pflatform);
void destroyPlatformImage(PlatformImage *pPlatformImage);

#endif