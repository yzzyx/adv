#ifndef COMMON_H
#define COMMON_H
#include <SDL.h>
#include <SDL_image.h>

#define TILE_WIDTH 32
#define TILE_HEIGHT 32

#define SCREEN_HEIGHT 640
#define SCREEN_WIDTH 640
#define TICK_TIMEOUT 10

#define RESOURCE_DIR "res/"

#define FRAMES_PER_SECOND 60

typedef struct t_render_space {
	SDL_Window *win;
	SDL_Surface *screen;
	SDL_Surface *map_surface;
	int fog_tile;
}render_space;

#endif /* end of include guard: COMMON_H */
