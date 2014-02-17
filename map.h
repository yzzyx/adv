#ifndef MAP_H
#define MAP_H
#include <Python.h>
#include "player.h"

typedef struct t_adv_tile {
	PyObject *py_obj;
	int animation_id;
	int animation_frame;

	int walkable;
}adv_tile;

typedef struct t_adv_map {
	PyObject *py_obj;
	int width;
	int height;
	adv_tile **tiles;
	int render_start_x;
	int render_start_y;
}adv_map;


typedef struct t_player player;
int render_map(adv_map *m, player *p);

#endif /* end of include guard: MAP_H */

