#include <SDL.h>
#include <SDL_image.h>
#include "common.h"
#include "animation.h"

#define FRAMES_PER_SECOND 60

render_space rs = { NULL, NULL, NULL, NULL, -1, -1, {-1,-1,-1,-1,-1,-1,-1,-1}, NULL };

SDL_Surface *surface_sdl(int w, int h)
{
	SDL_Surface *surface;
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	surface = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
	if(surface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		return NULL;
	}
	return surface;
}

int setup_sdl(int window_size_x, int window_size_y)
{

	//Start SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return -1;
	}

	rs.win = SDL_CreateWindow("adv",
	    SDL_WINDOWPOS_UNDEFINED,
	    SDL_WINDOWPOS_UNDEFINED,
	    window_size_x, window_size_y,
	    SDL_WINDOW_SHOWN);
	if (rs.win == NULL) {
		return -1;
	}

	if ((rs.real_screen = SDL_GetWindowSurface(rs.win)) == NULL) {
		return -1;
	}

	if ((rs.screen = surface_sdl(640,480)) == NULL) {
		return -1;
	}
	SDL_SetSurfaceBlendMode(rs.screen, SDL_BLENDMODE_NONE);
	printf("rs.screen in setup_sdl(): %p\n", rs.screen);
	return 0;
}
