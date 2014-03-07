#include "sdl.h"
#include "animation.h"
#include "player.h"
#include "map.h"
#include "python.h"
#include "astar.h"
#include "gamestate.h"

player *main_player;

#define FPS_MAX 60

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


int
mouse_move_event(SDL_Event *ev)
{
	int tx, ty;
	map_get_tile_position_from_screen(ev->motion.x, ev->motion.y, &tx, &ty);

	main_player->attack_target_x = tx;
	main_player->attack_target_y = ty;
	return 0;
}

int
attack_event(SDL_Event *ev)
{

	int tx, ty;

	map_get_tile_position_from_screen(ev->button.x, ev->button.y, &tx, &ty);
	printf("button pressed @ %d,%d == tile %d,%d\n", ev->button.x, ev->button.y, tx, ty);

	monster_attack(main_player, tx, ty);
	return 0;
}

int main(int argc, char *argv[])
{
	player *p;
	adv_map *m;
	
	if (setup_sdl(1280,960) == -1) {
		printf("setup_sdl(): -1\n");
		return -1;
	}
	
	animation_init();
	setup_python(argc, argv);

	rs.attack_cursor_sprites = animation_load_spritesheet("./img/attackcursor.png");

	int attack_sprites = animation_load_spritesheet("./img/attackanimation.png");
	animation_set_spritesheet_blendmode(attack_sprites, SDL_BLENDMODE_BLEND);
	rs.attack_animations[DIRECTION_DOWN]  = animation_create(attack_sprites, 0, 6);
	rs.attack_animations[DIRECTION_RIGHT] = animation_create(attack_sprites, 6, 6);
	rs.attack_animations[DIRECTION_UP]    = animation_create(attack_sprites, 12, 6);
	rs.attack_animations[DIRECTION_LEFT]  = animation_create(attack_sprites, 18, 6);
	rs.attack_animations[4] = animation_create(attack_sprites, 24, 6);
	rs.attack_animations[5] = animation_create(attack_sprites, 30, 6);
	rs.attack_animations[6] = animation_create(attack_sprites, 36, 6);
	rs.attack_animations[7] = animation_create(attack_sprites, 42, 6);

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

	p->xx = p->tile_x * SPRITE_SIZE;
	p->yy = p->tile_y * SPRITE_SIZE;
	p->target_tile_x = p->tile_x;
	p->target_tile_y = p->tile_y;
	p->map = m;
	printf("main: player animation: %d\n", p->animation_stopped[0]);

	main_player = p;

	int quit = 0;
	SDL_Event event;
	SDL_Rect screen_clip;
	SDL_Rect real_screen_clip;

	render_map(m, p);

	screen_clip.x = 0;
	screen_clip.y = 0;
	screen_clip.w = rs.screen->w;
	screen_clip.h = rs.screen->h;

	real_screen_clip.x = 0;
	real_screen_clip.y = 0;
	real_screen_clip.w = rs.real_screen->w;
	real_screen_clip.h = rs.real_screen->h;

	SDL_BlitScaled(rs.screen, &screen_clip, rs.real_screen, &real_screen_clip);
	SDL_UpdateWindowSurface(rs.win);

	while(!quit && main_player->hp > 0) {

		if (fps_limit())
			fps_update();

		gamestate_update();
		if (p->is_dirty) py_update_monster_from_c(p);
		update_map_monsters(m);
		if (render_map(m, p)) {
			SDL_BlitScaled(rs.screen, &screen_clip, rs.real_screen, &real_screen_clip);
			SDL_UpdateWindowSurface(rs.win);
		}

//		render_map_monsters(m);
//		render_map_objects(m);

		if (fps_data.frame_count % (10) == 1) {
			if (p->draw_movement) {
				int dir;
				if (p->has_directions) dir = p->direction;
				else dir = 0;
				animation_next_clip(p->animation_moving[dir]);

				if (!p->in_movement)
					p->draw_movement = 0;
			}


			/* FIXME! - these animations should have their own
			 *  frameset, so that the same base-animation
			 *  can be played multiple times simultaneously,
			 *  and they should also have their own timers
			 */
			adv_animation_list *prev_anim;
			adv_animation_list *anim;

			anim = rs.animation_list;
			prev_anim = NULL;
			while (anim) {
				if (animation_next_clip(anim->id) == 1) {
					/* Animation finished */
					if (prev_anim == NULL)
						rs.animation_list = anim->next;
					else
						prev_anim->next = anim->next;
					adv_animation_list *t;
					t = anim;
					anim = anim->next;
					free(t);
					continue;
				}
				anim = anim->next;
			}

			map_update_monster_animations(global_GS.current_map);
		}

		if (fps_data.frame_count % (FPS_MAX / 3) == 1) {
			py_update_object_timer((adv_base_object*)p);
			call_tick_map(m);
			call_tick_map_monsters(m);
			call_tick_map_objects(m);
		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
			       quit = 1;
			}

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					attack_event(&event);
				}
			}
			if (event.type == SDL_MOUSEMOTION) {
				mouse_move_event(&event);
			}
		}


		const uint8_t *keystate = SDL_GetKeyboardState(NULL);
		if ((keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]) && p->in_movement == 0)
			monster_goto_direction(p, DIRECTION_UP);

		if ((keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]) && p->in_movement == 0)
			monster_goto_direction(p, DIRECTION_DOWN);

		if ((keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]) && p->in_movement == 0)
			monster_goto_direction(p, DIRECTION_LEFT);

		if ((keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]) && p->in_movement == 0)
			monster_goto_direction(p, DIRECTION_RIGHT);

		if (keystate[SDL_SCANCODE_Q]) quit++;
		if (keystate[SDL_SCANCODE_M]) {
			monster_goto_position(m->monsters, 5, 5);
		}
		
		monster_move(p);
	}
	SDL_Quit();
	return 0;
}
