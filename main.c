#include "sdl.h"
#include "animation.h"
#include "player.h"
#include "map.h"
#include "python.h"
#include "astar.h"

player *main_player;

int main(int argc, char *argv[])
{
	player *p;
	adv_map *m;
	
	if (setup_sdl() == -1) {
		printf("setup_sdl(): -1\n");
		return -1;
	}
	
	animation_init();
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


	pathfinder_setup(m->width, m->height);

	p->xx = p->tile_x * FRAME_WIDTH;
	p->yy = p->tile_y * FRAME_HEIGHT;
	p->target_tile_x = p->tile_x;
	p->target_tile_y = p->tile_y;
	p->map = m;
	printf("main: player animation: %d\n", p->animation);

	animation_set_spritesheet_blendmode(animation_get_spritesheet_from_anim(p->animation),
	    SDL_BLENDMODE_BLEND);
	main_player = p;

	uint32_t ticks_last = SDL_GetTicks();
	int quit = 0;
	SDL_Event event;

	render_map(m, p);
	SDL_UpdateWindowSurface(rs.win);

	while(!quit) {

		update_map_monsters(m);
		move_player(p);
		if (render_map(m, p)) {
			SDL_UpdateWindowSurface(rs.win);
		}

//		render_map_monsters(m);
//		render_map_objects(m);
		
		//If we want to cap the frame rate
		if ((SDL_GetTicks() - ticks_last) >  (1000 / 2)) {
			//Sleep the remaining frame time
//			SDL_Delay((1000 / FRAMES_PER_SECOND) - (SDL_GetTicks() - ticks_last));
			ticks_last = SDL_GetTicks();

			py_update_object_timer((adv_base_object*)p);
			call_tick_map(m);
			call_tick_map_monsters(m);
			call_tick_map_objects(m);
		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
			       quit = 1;
			}
		}


		const uint8_t *keystate = SDL_GetKeyboardState(NULL);
		if (keystate[SDL_SCANCODE_UP] && p->in_movement == 0) { p->target_tile_y = p->tile_y - 1; }
		if (keystate[SDL_SCANCODE_DOWN] && p->in_movement == 0) { p->target_tile_y = p->tile_y + 1; }
		if (keystate[SDL_SCANCODE_LEFT] && p->in_movement == 0) { p->target_tile_x = p->tile_x - 1; }
		if (keystate[SDL_SCANCODE_RIGHT] && p->in_movement == 0) { p->target_tile_x = p->tile_x + 1; }
		if (keystate[SDL_SCANCODE_Q]) quit++;
		if (keystate[SDL_SCANCODE_M]) {

			monster_gotoPosition(p, 5, 5);
/*
		path =  pathfinder(m, p->tile_x, p->tile_y, 5, 5);
		printf("path: %s\n", path);
		free(path);
		*/
		}
	}
	SDL_Quit();
	return 0;
}
