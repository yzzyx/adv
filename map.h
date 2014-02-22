#ifndef MAP_H
#define MAP_H
#include <Python.h>
#include <SDL.h>
#include "player.h"

typedef struct t_adv_tile {
	PyObject *py_obj;
	int animation_id;
	int animation_frame;

	int walkable;
	int visibility;
}adv_tile;

typedef struct t_adv_map {
	PyObject *py_obj;
	int width;
	int height;

	SDL_Surface *map_surface;
	SDL_Surface *fog_surface;
	adv_tile **tiles;

	uint8_t *fog_map;
	int render_start_x;
	int render_start_y;
}adv_map;


typedef struct t_player player;
adv_map *get_map(const char *map_name);
int render_map(adv_map *m, player *p);

#endif /* end of include guard: MAP_H */

