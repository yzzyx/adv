#ifndef COMMON_H
#define COMMON_H
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

#define TILE_WIDTH 32
#define TILE_HEIGHT 32

#define SCREEN_HEIGHT 640
#define SCREEN_WIDTH 640
#define TICK_TIMEOUT 10

#define RESOURCE_DIR "res/"

#define FRAMES_PER_SECOND 60

typedef struct t_render_space {
	SDL_Surface *screen;
	TTF_Font *base_font;
}render_space;

#endif /* end of include guard: COMMON_H */
