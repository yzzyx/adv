#ifndef PLAYER_H
#define PLAYER_H
#include "common.h"

typedef struct t_adv_map adv_map;
typedef struct t_adv_monster {
	ADV_OBJECT_HEADER
	ADV_OBJECT_LIST

	int xx, yy; /* The absolute position of the monster */
	int tile_x; /* Position on map */
	int tile_y;
	int in_movement;

	/* Where is it heading? */
	int target_tile_x;
	int target_tile_y;

	int hp;
	int mp;
	int speed;

	int spritesheet;
	int animation;

	adv_map *map;
}adv_monster;

typedef struct t_adv_monster player;

player *setup_player();
int move_player(player *p);
int monster_gotoPosition(player *p, int x, int y);

extern player *main_player;

#endif /* end of include guard: PLAYER_H */

