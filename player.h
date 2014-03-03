#ifndef PLAYER_H
#define PLAYER_H
#include "common.h"

typedef struct t_adv_map adv_map;
typedef struct t_adv_monster {
	ADV_OBJECT_HEADER
	ADV_OBJECT_LIST

	int mod_x, mod_y;
	int xx, yy; /* The absolute position of the monster */
	int tile_x; /* Position on map */
	int tile_y;
	int in_movement;
	int draw_movement;

	/* Where is it heading? */
	int target_tile_x;
	int target_tile_y;

	int hp;
	int mp;
	int speed;

	int spritesheet;
	int animation_stopped[4];
	int animation_moving[4];

	adv_map *map;
}adv_monster;

typedef struct t_adv_monster player;

player *setup_player();
int move_player(player *p);
int monster_gotoPosition(player *p, int x, int y);
adv_monster *monster_get_from_pyobj(PyObject *py);
int monster_position_is_visible(adv_monster *m, int x, int y);

extern player *main_player;

#endif /* end of include guard: PLAYER_H */

