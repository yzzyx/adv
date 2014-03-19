#ifndef ADV_SDL_H
#define ADV_SDL_H
#include "common.h"

extern render_space rs;
SDL_Surface *surface_sdl(int w, int h);
int setup_sdl();
int sdl_resize_event(SDL_Event *ev);

#endif /* end of include guard: ADV_SDL_H */

