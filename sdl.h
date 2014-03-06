#ifndef ADV_SDL_H
#define ADV_SDL_H
#include "common.h"

extern render_space rs;
SDL_Surface *surface_sdl(int w, int h);
int setup_sdl(int window_size_x, int window_size_y);

#endif /* end of include guard: ADV_SDL_H */

