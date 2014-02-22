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

	if ((m= get_map("level1")) == NULL) {
		printf("No map!\n");
		SDL_Quit();
		return -1;
	}

	if ((p = setup_player()) == NULL) {
		SDL_Quit();
		return -1;
	}

	p->xx = p->tile_x * FRAME_WIDTH;
	p->yy = p->tile_y * FRAME_HEIGHT;
	p->map = m;
	set_animation_blendmode(p->animation_id, SDL_BLENDMODE_BLEND);

	uint32_t ticks_last = 0;
	int quit = 0;
	SDL_Event event;

	render_map(m, p);
	SDL_UpdateWindowSurface(rs.win);

	while(!quit) {
		if (move_player(p)) {
			render_map(m, p);
			SDL_UpdateWindowSurface(rs.win);
		}

		//If we want to cap the frame rate
		if ((SDL_GetTicks()<  1000 / FRAMES_PER_SECOND)) {
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
		}
	}
	SDL_Quit();
	return 0;
}
