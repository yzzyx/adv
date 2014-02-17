#ifndef PLAYER_H
#define PLAYER_H
#include "python.h"

typedef struct t_player {
	int tile_x;
	int tile_y;

	int movement_x;
	int movement_y;
	int xx, yy;
	int in_movement;

	int hp;
	int mp;
	int speed;
	int animation_id;
	int animation_frame;

	PyObject *py_obj;
}player;

typedef struct t_adv_map adv_map;

player *setup_player();
int move_player(adv_map *m, player *p);

#endif /* end of include guard: PLAYER_H */

