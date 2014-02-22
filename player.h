#ifndef PLAYER_H
#define PLAYER_H
#include "python.h"

typedef struct t_adv_map adv_map;
typedef struct t_player {

	int xx, yy; /* The absolute position of the monster */
	int tile_x; /* Position on map */
	int tile_y;

	/* Movement - which direction, and what the target is */
	int target_tile_x;
	int target_tile_y;
	int movement_x;
	int movement_y;
	int in_movement;

	int hp;
	int mp;
	int speed;
	int animation_id;
	int animation_frame;

	adv_map *map;
	PyObject *py_obj;
}player;

typedef struct t_adv_map adv_map;

player *setup_player();
int move_player(player *p);

int monster_gotoPos_C(player *p, int x, int y);

#endif /* end of include guard: PLAYER_H */

