#include "sdl.h"
#include "animation.h"
#include "map.h"
#include "player.h"
#include "python.h"

int main(int argc, char *argv[])
{
	player *p;
	adv_map *m;

	
	if (setup_sdl(argc, argv) == -1) {
		printf("setup_sdl(): -1\n");
		return -1;
	}
	
#if 0
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return -1;
	}


	if ((rs.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE)) == NULL) {
		return -1;
	}

	SDL_WM_SetCaption("Adv", NULL);
#endif

	setup_animation();
	setup_python(argc, argv);

	m = python_generate_map("level1");
//	printf("map: %d x %d\n", m->width, m->height);
	if (!m) {
		printf("No map!\n");
		SDL_Quit();
		return -1;
	}

	if ((p = setup_player()) == NULL) {
		SDL_Quit();
		return -1;
	}

	uint32_t ticks_last = 0;
	int quit = 0;
	uint32_t black = 0;
	SDL_Event event;
	while(!quit) {
		
		SDL_FillRect(rs.screen, NULL, black);
		render_map(m, p);
		SDL_Flip(rs.screen);

		//If we want to cap the frame rate
		if ((SDL_GetTicks() < 1000 / FRAMES_PER_SECOND)) {
			//Sleep the remaining frame time
			SDL_Delay((1000 / FRAMES_PER_SECOND) - SDL_GetTicks());
		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
			       quit = 1;
			}
		}

		uint32_t t = SDL_GetTicks();
		if (t - ticks_last > TICK_TIMEOUT) {
			ticks_last = t;


			uint8_t *keystate = SDL_GetKeyState(NULL);
			if (keystate[SDLK_UP] && p->in_movement == 0) p->movement_y = -1;
			if (keystate[SDLK_DOWN] && p->in_movement == 0) p->movement_y = +1;
			if (keystate[SDLK_LEFT] && p->in_movement == 0) p->movement_x = -1;
			if (keystate[SDLK_RIGHT] && p->in_movement == 0) p->movement_x = +1;
			if (keystate[SDLK_q]) quit++;
			/*
			if (p.y < 0) p.y = 0;
			if (p.y >= m->height) p.y = m->height - 1;
			if (p.x < 0) p.x = 0;
			if (p.x >= m->width) p.x = m->width - 1;
			*/
			move_player(m, p);
		}
	}

	SDL_Quit();
	return 0;

}
