#ifndef MAP_H
#define MAP_H
#include <Python.h>
#include <SDL.h>
#include "common.h"
#include "player.h"

typedef struct t_adv_map adv_map;
typedef struct t_adv_tile {
	ADV_OBJECT_HEADER

	int animation_id;
	int animation_frame;

	int walkable;
	int visibility;
}adv_tile;

typedef struct t_adv_object {
	ADV_OBJECT_HEADER
	ADV_OBJECT_LIST

	int animation_id;
	int animation_frame;
}adv_object;

typedef struct t_adv_map {
	ADV_OBJECT_HEADER

	int width;
	int height;

	SDL_Surface *map_surface;
	SDL_Surface *fog_surface;
	adv_tile **tiles;
	adv_monster *monsters;
	adv_object *objects;

	uint8_t *fog_map;
	int render_start_x;
	int render_start_y;
}adv_map;

adv_map *get_map(const char *map_name);
int render_map(adv_map *m, player *p);

int update_map_monsters(adv_map *m);
int call_tick_map(adv_map *m);
int call_tick_map_monsters(adv_map *m);
int call_tick_map_objects(adv_map *m);

#endif /* end of include guard: MAP_H */

