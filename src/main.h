#ifndef MAIN_H
#define MAIN_H

#define WIDTH 1280
#define HEIGHT 720
#define FPS 60.0f
#define FRAME_DELAY_MS (1000.0f / FPS)

#include <SDL3/SDL.h>

#define G 100.0f
#define EPS 1e-4f

extern SDL_Renderer *rend;

#endif
