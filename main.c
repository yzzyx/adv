#include "sdl.h"
#include "animation.h"
#include "map.h"
#include "player.h"
#include "python.h"

int main(int argc, char *argv[])
{
	player *p;
	adv_map *m;

	
	if (setup_sdl() == -1) {
		printf("setup_sdl(): -1\n");
		return -1;
	}
	
	setup_animation();
	setup_python(argc, argv);

	m = python_generate_map("level1");
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
	SDL_Event event;

//	SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_ALPHA);
//	render_animation_full(2,0,0,0,surface);
	while(!quit) {
		
		/*
		render_animation_full(2, 0, 0, 0, m->map_surface);
		SDL_Rect clip;
		clip.x = 0;
		clip.y = 0;
		clip.w = rs.screen->w;
		clip.h = rs.screen->h;

		*/
		//render_animation_full(rs.fog_tile, 0, 0, 0, rs.screen);
		render_map(m, p);
		SDL_UpdateWindowSurface(rs.win);

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
		if (1 || t - ticks_last > TICK_TIMEOUT) {
			ticks_last = t;


			const uint8_t *keystate = SDL_GetKeyboardState(NULL);
			if (keystate[SDL_SCANCODE_UP] && p->in_movement == 0) p->movement_y = -1;
			if (keystate[SDL_SCANCODE_DOWN] && p->in_movement == 0) p->movement_y = +1;
			if (keystate[SDL_SCANCODE_LEFT] && p->in_movement == 0) p->movement_x = -1;
			if (keystate[SDL_SCANCODE_RIGHT] && p->in_movement == 0) p->movement_x = +1;
			if (keystate[SDL_SCANCODE_Q]) quit++;
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
