#include "sdl.h"
#include "animation.h"
#include "player.h"
#include "map.h"
#include "python.h"
#include "astar.h"
#include "gamestate.h"

player *main_player;

#define FPS_MAX 10

struct fps_data
{
	int fps;

	uint32_t curr_ticks;
	float max_frame_ticks;
	uint32_t last_second_ticks;
	int frame_count;
	
	uint32_t last_frame_ticks;
	uint32_t target_ticks;
}fps_data;

void fps_init()
{ 
	fps_data.max_frame_ticks=(1000.0/(float)FPS_MAX)+0.00001;
	fps_data.frame_count=0;
	fps_data.last_second_ticks=SDL_GetTicks();
}

int fps_limit()
{
	fps_data.frame_count++;
	fps_data.target_ticks = fps_data.last_second_ticks +
	    (uint32_t)(fps_data.frame_count * fps_data.max_frame_ticks);
	fps_data.curr_ticks = SDL_GetTicks();
	
	if (fps_data.curr_ticks < fps_data.target_ticks) {
		SDL_Delay(fps_data.target_ticks - fps_data.curr_ticks);
		fps_data.curr_ticks = SDL_GetTicks();
	}

	fps_data.last_frame_ticks = fps_data.curr_ticks;
	if (fps_data.curr_ticks - fps_data.last_second_ticks>=1000) {
		fps_data.fps = fps_data.frame_count;
		fps_data.frame_count=0;
		fps_data.last_second_ticks = SDL_GetTicks();
		return 1;
	}
	return 0;
}


void
fps_update()
{
	char str[255];
	sprintf(str, "adv - %d/%d", fps_data.fps, FPS_MAX);
	SDL_SetWindowTitle(rs.win, str);
}

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

	if ((m=get_map("level1")) == NULL) {
		printf("No map!\n");
		SDL_Quit();
		return -1;
	}

	if ((p = setup_player()) == NULL) {
		SDL_Quit();
		return -1;
	}

	gamestate_init();
	fps_init();
	pathfinder_setup(m->width, m->height);

	global_GS.current_map = m;

	p->xx = p->tile_x * FRAME_WIDTH;
	p->yy = p->tile_y * FRAME_HEIGHT;
	p->target_tile_x = p->tile_x;
	p->target_tile_y = p->tile_y;
	p->map = m;
	printf("main: player animation: %d\n", p->animation_stopped[0]);

	main_player = p;

	int quit = 0;
	SDL_Event event;

	render_map(m, p);
	SDL_UpdateWindowSurface(rs.win);

	while(!quit) {

		if (fps_limit())
			fps_update();

		gamestate_update();
		if (p->is_dirty) py_update_monster_from_c(p);
		update_map_monsters(m);
		if (render_map(m, p)) {
			SDL_UpdateWindowSurface(rs.win);
		}

//		render_map_monsters(m);
//		render_map_objects(m);
		move_player(p);

		if (fps_data.frame_count % (FPS_MAX / 3) == 1) {
			if (p->in_movement) {
				animation_next_clip(p->animation_moving[0]);
				if (p->has_directions) {
					animation_next_clip(p->animation_moving[1]);
					animation_next_clip(p->animation_moving[2]);
					animation_next_clip(p->animation_moving[3]);
				}
			}

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
		if ((keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]) &&
		    p->in_movement == 0) { p->target_tile_y = p->tile_y - 1; }
		if ((keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]) &&
		    p->in_movement == 0) { p->target_tile_y = p->tile_y + 1; }
		if ((keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]) &&
		    p->in_movement == 0) { p->target_tile_x = p->tile_x - 1; }
		if ((keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]) &&
		    p->in_movement == 0) { p->target_tile_x = p->tile_x + 1; }
		if (keystate[SDL_SCANCODE_Q]) quit++;
		if (keystate[SDL_SCANCODE_M]) {

			monster_gotoPosition(p, 5, 5);
		}
	}
	SDL_Quit();
	return 0;
}
