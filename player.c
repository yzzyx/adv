#include <Python.h>
#include "animation.h"
#include "player.h"
#include "common.h"
#include "astar.h"
#include "map.h"
#include "python.h"
#include "gamestate.h"

#define TICK_LENGTH 3

/*
 * py_update_c_player()
 *
 * Copy information from python to C
 */
int
py_update_c_player(player *p)
{

	py_update_monster((adv_monster*)p);
	return 0;
}

player *
setup_player()
{
	player *p;

	p = malloc(sizeof *p);
	memset(p, 0, sizeof *p);

	PyObject *module = PyDict_GetItemString(main_dict, "player");
	if (module == NULL) {
		printf("PyDict_GetItemString():\n");
		PyErr_Print();
		free(p);
		return NULL;
	}
	PyObject *obj_def = PyObject_GetAttrString(module, "Player");
	if (obj_def == NULL) {
		printf("PyDict_GetAttrString():\n");
		PyErr_Print();
		return NULL;
	}

	PyObject *obj_inst = PyObject_CallObject(obj_def, NULL);
	if (obj_inst == NULL) {
		printf("player::PyObject_CallObject():\n");
		PyErr_Print();
		return NULL;
	}

	p->py_obj = obj_inst;
	py_update_c_player(p);
	return p;
}

/* collision_player(player, map)
 *
 * Check for collision
 */
int
player_map_is_walkable(player *p)
{
	int tx, ty;
	int min_x, min_y, max_x, max_y;

	min_x = p->xx / FRAME_WIDTH;
	min_y = p->yy / FRAME_WIDTH;

	max_x = min_x + (((p->xx % FRAME_WIDTH) > 0) ? 1:0);
	max_y = min_y + (((p->yy % FRAME_WIDTH) > 0) ? 1:0);
	if (max_x >= p->map->width) max_x = p->map->width - 1;
	if (max_y >= p->map->height) max_y = p->map->height - 1;

	for (tx = min_x; tx <= max_x; tx ++) {
		for (ty = min_y; ty <= max_y; ty ++) {
			if (!p->map->tiles[tx+ty*p->map->width]->walkable)
				return 0;
		}
	}
			
	return 1;

}

/* move_player
 *
 * called each tick, updating the player's position
 */
int move_player(player *p)
{
	int dir;
	int new_tile = 0;
	int prev_tile_x, prev_tile_y;
	int mx = 0, my = 0;
	PyObject *tmp;

	prev_tile_x = p->tile_x;
	prev_tile_y = p->tile_y;

	/* check for errors */
	if (p->target_tile_x < 0) p->target_tile_x = 0;
	if (p->target_tile_y < 0) p->target_tile_y = 0;
	if (p->target_tile_x > p->map->width - 1) p->target_tile_x = p->map->width - 1;
	if (p->target_tile_y > p->map->height - 1) p->target_tile_y = p->map->height - 1;

	printf("%d,%d ---> %d,%d\n", p->tile_x, p->tile_y, p->target_tile_x,
	    p->target_tile_y);
	if (p->target_tile_x == p->tile_x  && p->target_tile_y == p->tile_y)
		return 0;

	dir = p->direction;
	/*
	if (map_is_walkable(p, p->map, p->target_tile_x, p->target_tile_y)) {
		if (p->target_tile_x == p->tile_x && p->target_tile_y == p->tile_y - 1)
			dir = DIRECTION_UP;
		else if (p->target_tile_x == p->tile_x  && p->target_tile_y == p->tile_y + 1)
			dir = DIRECTION_DOWN;
		else if (p->target_tile_x == p->tile_x - 1 && p->target_tile_y == p->tile_y)
			dir = DIRECTION_LEFT;
		else if (p->target_tile_x == p->tile_x + 1 && p->target_tile_y == p->tile_y)
			dir = DIRECTION_RIGHT;
	} else {
	*/
		int x2, y2;

		x2 = p->target_tile_x;
		y2 = p->target_tile_y;
		int dir_chr = pathfinder(p, p->tile_x, p->tile_y, &x2, &y2);

		if (x2 != p->target_tile_x ||
		    y2 != p->target_tile_y) {
			printf("target changed\n");
			p->target_tile_x = x2;
			p->target_tile_y = y2;
			p->is_dirty = 1;
		}

		if (p->target_tile_x == p->tile_x  && p->target_tile_y == p->tile_y)
			return 0;

		/* FIXME - store path somewhere, and don't use these conversions */
		if (dir_chr == 'N') { dir = DIRECTION_UP; }
		else if (dir_chr == 'S') { dir = DIRECTION_DOWN; }
		else if (dir_chr == 'E') { dir = DIRECTION_LEFT; }
		else if (dir_chr == 'W') { dir = DIRECTION_RIGHT; }
	//}

	if (dir == DIRECTION_UP) { my = -1; }
	else if (dir == DIRECTION_DOWN) { my = 1; }
	else if (dir == DIRECTION_RIGHT) { mx = 1; }
	else if (dir == DIRECTION_LEFT) { mx = -1; }
	p->direction = dir;

	if (!p->in_movement) {
	    p->in_movement = 1;

	    /* Call playerExit-method on tile we're leaving*/
	    tmp = PyObject_CallMethod(
		p->map->tiles[prev_tile_x + prev_tile_y * p->map->width]->py_obj,
		"playerExit", "", NULL);
	    if (tmp == NULL) { PyErr_Print(); return -1; }
	    Py_DECREF(tmp);
	}

	p->tile_x += mx;
	p->tile_y += my;
	p->xx = p->tile_x * SPRITE_SIZE;
	p->yy = p->tile_y * SPRITE_SIZE;
	new_tile = 1;

	if (new_tile) {
		/* Call playerEnter-method on tile we're entering*/
		tmp = PyObject_CallMethod(p->map->tiles[p->tile_x + p->tile_y * p->map->width]->py_obj,
		    "playerEnter", "", NULL);
		if (tmp == NULL) { PyErr_Print(); return -1; }
		Py_DECREF(tmp);

		p->is_dirty = 1;
	}

	if (p->target_tile_x == p->tile_x &&
	    p->target_tile_y == p->tile_y)
		p->in_movement = 0;

	return 1;
}

int
monster_gotoPosition(player *p, int x, int y)
{
	p->target_tile_x = x;
	p->target_tile_y = y;
	p->is_dirty = 1;

	return 0;
}

adv_monster *
monster_get_from_pyobj(PyObject *py_obj)
{
	adv_monster *monster;

	monster = global_GS.current_map->monsters;
	for (; monster != NULL; monster = (adv_monster *)monster->next) {
		if (monster->py_obj == py_obj)
			return monster;
	}
	return NULL;
}

int
monster_position_is_visible(adv_monster *m, int map_x, int map_y)
{
	int dx = 0, dy = 0;
	int x,y;
	int sx, sy;
	int err;
	
	dx = abs(m->tile_x-map_x);
	dy = abs(m->tile_y-map_y);
	if (map_x < m->tile_x) sx = 1; else sx = -1;
	if (map_y < m->tile_y) sy = 1; else sy = -1;
	err = dx-dy;
	x = map_x;
	y = map_y;

	for(;;) {
		if (global_GS.current_map->tiles[x+y*
		    global_GS.current_map->width]->visibility == 0) {
			if (!(x == map_x && y == map_y))
				return 0;
		}

		if (x == m->tile_x && y == m->tile_y)
			break;
		int e2 = 2 * err;
		if (e2 > -dy) {
			err = err - dy;
			x += sx;
		}
		if (x == m->tile_x && y == m->tile_y) {
//			if (m->tiles[x+y*m->width]->visibility == 0)
//				return 0;
			break;
		}
		if (e2 < dx) {
			err = err + dx;
			y += sy;
		}
	}
	return 1;
}
