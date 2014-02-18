#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "common.h"
#include "animation.h"

#define FRAMES_PER_SECOND 60

render_space rs;

#if 0
void base_tile::render(render_space *rs, int x, int y)
{
	map_animation->render(rs, x, y);
}

void map::render(render_space *rs)
{
	/* first, render all tiles */
	for (int x = 0; x < width; x ++)
		for (int y = 0; y < height; y ++)
			tiles[x+y*width]->render(rs, x * TILE_WIDTH, y * TILE_HEIGHT);
}
	/* render all objects */


#endif

int setup_sdl(int argc, char* args[])
{

	SDL_Event event;
	int quit = 0;
	int cap = 1;
	
	//Start SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return -1;
	}

	if ((rs.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE)) == NULL) {
		return -1;
	}
	printf("rs.screen in setup_sdl(): %p\n", rs.screen);

	/*
	if (TTF_Init() == -1) {
		return -1;
	}
	*/

	SDL_WM_SetCaption("Adv", NULL);

	/* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
	   as expected by OpenGL for textures */
	Uint32 rmask, gmask, bmask, amask;

	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
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

	rs.map_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, rs.screen->w + FRAME_WIDTH, rs.screen->h + FRAME_HEIGHT, 32,
	    rmask, gmask, bmask, amask);
	if(rs.map_surface == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		return -1;
	}
	return 0;

/*	rs.base_font = TTF_OpenFont("font.ttf", 28);
	if (rs.base_font == NULL) {
		return -1;
	}
	*/


#if 0
	level1 = new map(10,10);
	for (int x = 0; x < 10; x ++)
		for (int y = 0; y < 10; y ++)
			level1->tiles[x+y*level1->width] = new grass_tile;

	delete level1->tiles[5+5*level1->width];
	level1->tiles[5+5*level1->width] = new boulder_tile;


	level1->render(&rs);
	level1->tiles[5+5*level1->width]->render(&rs, 1, 1);

	SDL_Flip(rs.screen);
	int x, y;

	x= 180, y=140;
	quit = 0;
	while(!quit) {
		 //If we want to cap the frame rate
		 if (cap && (SDL_GetTicks() < 1000 / FRAMES_PER_SECOND)) {
			 //Sleep the remaining frame time
			 SDL_Delay((1000 / FRAMES_PER_SECOND) - SDL_GetTicks());
		 }
		 //

		SDL_Flip(rs.screen);

		 //While there's an event to handle
		 while (SDL_PollEvent(&event)) {
			 if (event.type == SDL_QUIT) {
				quit = 1;
			 }
		 }

		 /*
		 uint32_t t = SDL_GetTicks();
		 if (t - ticks_last > TICK_TIMEOUT) {
			 ticks_last = t;
			 uint8_t *keystate = SDL_GetKeyState(NULL);
			 if (keystate[SDLK_UP]) y -= 5;
			 if (keystate[SDLK_DOWN]) y += 5;
			 if (keystate[SDLK_LEFT]) x -= 5;
			 if (keystate[SDLK_RIGHT]) x += 5;
			 if (y < 0) y = 0;
			 if (y > SCREEN_HEIGHT - message->h) y = SCREEN_HEIGHT -
				 message->h;
			 if (x < 0) x = 0;
			 if (x > SCREEN_WIDTH - message->w) x = SCREEN_WIDTH -
				 message->w;
		 }
		 */

	}
	/*
	TTF_CloseFont(rs.base_font);
	TTF_Quit();
	*/

	//Quit SDL
	SDL_Quit();
	return 0;
#endif
	return 0;
}
