#include <math.h>
#include "astar.h"
#include "python.h"
#include "animation.h"
#include "sdl.h"
#include "player.h"
#include "common.h"
#include "map.h"
#include "python.h"
#include "gamestate.h"

#define PI 3.14159265358979323846

PyObject *
setup_player()
{
	PyObject *module = PyDict_GetItemString(main_dict, "player");
	if (module == NULL) {
		printf("PyDict_GetItemString():\n");
		PyErr_Print();
		return NULL;
	}
	PyObject *obj_def = PyObject_GetAttrString(module, "Player");
	if (obj_def == NULL) {
		printf("PyDict_GetAttrString():\n");
		PyErr_Print();
		return NULL;
	}

	PyObject *p = PyObject_CallObject(obj_def, NULL);
	if (p == NULL) {
		printf("player::PyObject_CallObject():\n");
		PyErr_Print();
		return NULL;
	}

	py_setattr_int(p, ATTR_DIRECTION, 0);
	py_setattr_int(p, ATTR_INT_TARGET_X, py_getattr_int(p, ATTR_X));
	py_setattr_int(p, ATTR_INT_TARGET_Y, py_getattr_int(p, ATTR_Y));
	return p;
}



/* move_player
 *
 * called each tick, updating the player's position
 */
int
monster_move(PyObject *monster)
{
	int x1, y1; /* current position */
	int x2, y2; /* target position */
	int speed;
	node_t *active_path;

	/* Check if we have an active path */
	active_path = (node_t*)py_getattr_int(monster, ATTR_INT_ACTIVE_PATH);
	if (active_path == 0)
		return 0;

	x1 = py_getattr_int(monster, ATTR_X);
	y1 = py_getattr_int(monster, ATTR_Y);
	x2 = active_path->x;
	y2 = active_path->y;
	speed = py_getattr_int(monster, ATTR_INT_SPEED);

	/* monster-move-int
	 * ----------------
	 */
	
	int dx = 0, dy = 0;
	int sx, sy;
	int err;
	int x, y;
	int i;

	dx = abs(x2-x1);
	dy = abs(y2-y1);
	if (x1 < x2) sx = 1; else sx = -1;
	if (y1 < y2) sy = 1; else sy = -1;
	err = dx-dy;
	x = x1;
	y = y1;

	for(i = 0;i < speed; i++) {
		if (x == x2 && y == y2)
			break;

		int e2 = 2 * err;
		if (e2 > -dy) {
			err = err - dy;
			x += sx;
		}
		if (x == x2 && y == y2) {
			break;
		}
		if (e2 < dx) {
			err = err + dx;
			y += sy;
		}
	}
	py_setattr_int(monster, ATTR_X, x);
	py_setattr_int(monster, ATTR_Y, y);

	/* We've reached our target */
	if (x == x2 && y == y2) {
		active_path = active_path->child;
		py_setattr_int(monster, ATTR_INT_ACTIVE_PATH, (long)active_path);
	}
	return 0;

#if 0
	/* FIXME! Call playerExit-method on tile we're leaving*/
	tmp = PyObject_CallMethod(
		p->map->tiles[prev_tile_x + prev_tile_y * p->map->width]->py_obj,
		"playerExit", "", NULL);
	if (tmp == NULL) { PyErr_Print(); return -1; }
	Py_DECREF(tmp);

	/* FIXME! Call playerEnter-method on tile we're entering*/
	tmp = PyObject_CallMethod(p->map->tiles[p->tile_x + p->tile_y * p->map->width]->py_obj,
		"playerEnter", "", NULL);
	if (tmp == NULL) { PyErr_Print(); return -1; }
	Py_DECREF(tmp);
	p->is_dirty = 1;
#endif
}

/*
 * monster_goto_direction(m, d)
 *
 * Try to make monster walk in direction "d".
 * Returns 1 if it's possible, or 0 otherwise
 */
int
monster_goto_direction(PyObject *p, int direction)
{
	int mx, my;
	int target_x, target_y;
	int curr_x, curr_y;
	int speed;
	node_t *active_path;
	int monster_sprite_width;
	int monster_sprite_height;

	active_path = (node_t*)py_getattr_int(p, ATTR_INT_ACTIVE_PATH);
	if (active_path) {
		while (active_path->child) {
			active_path = active_path->child;
		}
		pathfinder_free_path(active_path);
	}

	curr_x = py_getattr_int(p, ATTR_X);
	curr_y = py_getattr_int(p, ATTR_Y);
	speed = py_getattr_int(p, ATTR_INT_SPEED);

	/* FIXME - allow monsters with different sprite-sizes */
	monster_sprite_width = 32;
	monster_sprite_height = 32;

	switch(direction) {
		case DIRECTION_UP: mx = 0; my = -1; break;
		case DIRECTION_DOWN: mx = 0; my = 1; break;
		case DIRECTION_LEFT: mx = -1; my = 0; break;
		case DIRECTION_RIGHT: mx = 1; my = 0; break;
		default:
			return 0;
			break;
	}

	int i;
	target_x = curr_x;
	target_y = curr_y;

	monster_sprite_width /= 2;
	monster_sprite_height /= 2;
	for (i = 0; i < speed; i ++) {
		target_x += mx;
		target_y += my;
		if (target_x < monster_sprite_width) {
			target_x = monster_sprite_width;
			break;
		}
		if (target_y < monster_sprite_height) {
			target_y = monster_sprite_height;
			break;
		}
		if (target_x >= global_GS.current_map->width - monster_sprite_width) {
			target_x = global_GS.current_map->width - monster_sprite_width - 1;
			break;
		}
		if (target_y >= global_GS.current_map->height - monster_sprite_height) {
			target_y = global_GS.current_map->height - monster_sprite_height - 1;
			break;
		}

		/* Don't even try to walk where we can't */
		if (global_GS.current_map->raytrace_map[target_x + target_y *
		    global_GS.current_map->width]) {
			target_x -= mx;
			target_y -= my;
			break;
		}
	}

	/*
	 * Create a path to this point
	 */
	active_path = calloc(1,sizeof(*active_path));
	active_path->child = NULL;
	active_path->parent = NULL;
	active_path->x = target_x;
	active_path->y = target_y;

	py_setattr_int(p, ATTR_INT_ACTIVE_PATH, (long)active_path);
	py_setattr_int(p, ATTR_IN_MOVEMENT, 1);

	return 1;
}


/*
 * monster_goto_position(m, x, y)
 *
 * Try to make monster walk to x, y
 * Returns 1 if it's possible, or 0 otherwise
 */
int
monster_goto_position(PyObject *monster, int x, int y)
{
	int monster_x, monster_y;
	node_t *active_path;

	/* don't walk outside of map */
	if (x < SPRITE_SIZE/2) x = SPRITE_SIZE/2;
	if (y < SPRITE_SIZE/2) y = SPRITE_SIZE/2;
	if (x >= global_GS.current_map->width - SPRITE_SIZE/2)
		x = global_GS.current_map->width - SPRITE_SIZE/2;
	if (y >= global_GS.current_map->height - SPRITE_SIZE/2)
		y = global_GS.current_map->height - SPRITE_SIZE/2;

	/* check if target position is even a valid position */
	if (global_GS.current_map->raytrace_map
		[x+y*global_GS.current_map->width] != 0) {
		return 0;
	}

	monster_x = py_getattr_int(monster, ATTR_X);
	monster_y = py_getattr_int(monster, ATTR_Y);

	/* Check if we already have an active path */
	active_path = (node_t*)py_getattr_int(monster, ATTR_INT_ACTIVE_PATH);
	if (active_path) {
		while (active_path->child) {
			active_path = active_path->child;
		}

		/* We've already got a path to this place, don't
		 * do anything
		 */
		if (active_path->x == x && active_path->y == y)
			return 0;

		/* We need a new path, get rid of the old one */
		pathfinder_free_path(active_path);
	}

	if (map_has_line_of_sight(monster_x, monster_y, x, y)) {

		/* We can walk directly to this point,
		 * so create a path to it
		 */
		active_path = calloc(1,sizeof(*active_path));
		active_path->child = NULL;
		active_path->parent = NULL;
		active_path->x = x;
		active_path->y = y;

		py_setattr_int(monster, ATTR_INT_ACTIVE_PATH, (long)active_path);
		py_setattr_int(monster, ATTR_IN_MOVEMENT, 1);
		return 0;
	}


	/* Note that we're working with grid-coordinates in the pathfinder,
	 * to speed things up, and make it less resource-hungry
	 */
	active_path = pathfinder(global_GS.current_map->pathfinder, monster,
	    monster_x / SPRITE_SIZE, monster_y / SPRITE_SIZE,
	    x / SPRITE_SIZE, y / SPRITE_SIZE); 

	if (!active_path) {
		printf("No path from %d,%d to %d,%d\n",
		    monster_x, monster_y, x, y);
		return 0;
	}

	/* Convert path from grid-coordinates to real coordinates */
	node_t *node;
	for (node = active_path; node != NULL; node = node->child) {
		node->x = node->x * SPRITE_SIZE + SPRITE_SIZE/2;
		node->y = node->y * SPRITE_SIZE + SPRITE_SIZE/2;
	}

	/* FIXME - prune path */

	py_setattr_int(monster, ATTR_INT_ACTIVE_PATH, (long)active_path);
	py_setattr_int(monster, ATTR_IN_MOVEMENT, 1);
	return 0;
}

int
monster_position_is_visible(PyObject *monster, int map_x, int map_y)
{
	int monster_x, monster_y;

	monster_x = py_getattr_int(monster, ATTR_X);
	monster_y = py_getattr_int(monster, ATTR_Y);
	return map_has_line_of_sight(monster_x, monster_y, map_x, map_y);
}

/*
 * monster_attack(m, x, y)
 *
 * Attack in direction of (x,y)
 * The actual outcome differs depending on weapons, etc.
 */
int
monster_attack(PyObject *m, int x, int y)
{
	/* FIXME - needs to be redone after free-walk patch */
	int dir = 0;
	int mx = 0, my = 0;
	int tile_x, tile_y;

	tile_x = py_getattr_int(m, ATTR_X);
	tile_y = py_getattr_int(m, ATTR_Y);

	float angle = atan2f(x - tile_x,
						 y - tile_y) * 180/PI;

	/* Straight directions */
	if (angle >   0 - 45.0/2 && angle <=   0 + 45.0/2) {
		dir = DIRECTION_DOWN;
		my = 1;
	} else if (angle >  90 - 45.0/2 && angle <=  90 + 45.0/2) {
		dir = DIRECTION_RIGHT;
		mx = 1;
	} else if (angle > 180 - 45.0/2 && angle <= 180 + 45.0/2) {
		dir = DIRECTION_UP;
		my = -1;
	} else if (angle > -90 - 45.0/2 && angle <= -90 + 45.0/2) {
		dir = DIRECTION_LEFT;
		mx = -1;

	/* Corner directions */
	} else if (angle >   45 - 45.0/2 && angle <=   45 + 45.0/2) { /* DOWN+RIGHT */
		dir = 4;
		mx = 1;
		my = 1;
	} else if (angle >  -45 - 45.0/2 && angle <=  -45 + 45.0/2) { /* DOWN+LEFT */
		dir = 5;
		mx = -1;
		my = 1;
	} else if (angle >  135 - 45.0/2 && angle <=  135 + 45.0/2) { /* UP+RIGHT */
		dir = 6;
		mx = 1;
		my = -1;
	} else if (angle > -135 - 45.0/2 && angle <= -135 + 45.0/2) { /* UP+LEFT */
		dir = 7;
		mx = -1;
		my = -1;
	}

	animation_play(rs.attack_animations[dir], tile_x + mx * SPRITE_SIZE, tile_y + my * SPRITE_SIZE);

	/* Check if player is standing there */
	/* FIXME - do this with new attr-code */
#if 0
	if (main_player->tile_x == m->tile_x + mx &&
		main_player->tile_y == m->tile_y + my) {

		printf("Attacking monster @ %d,%d with hp: %d\n", main_player->tile_x,
			main_player->tile_y, main_player->hp);
		main_player->hp -= 20;
		main_player->is_dirty = 1;
		py_update_monster_from_c(main_player);

		if (main_player->hp <= 0) {
			PyObject *tmp;
			tmp = PyObject_CallMethod(main_player->py_obj, "isDead", NULL);
			if (tmp != NULL)
				Py_DECREF(tmp);
			return 0;
		}

		PyObject *tmp;
		tmp = PyObject_CallMethod(main_player->py_obj, "isHit", NULL);
		if (tmp != NULL)
			Py_DECREF(tmp);
	}

	/* Check if any monster is standing there */
	adv_monster *monster;
	monster = global_GS.current_map->monsters;


	for (; monster != NULL; monster = (adv_monster *)monster->next) {

		if (monster->tile_x == m->tile_x + mx &&
			monster->tile_y == m->tile_y + my) {


			printf("Attacking monster @ %d,%d with hp: %d\n", monster->tile_x,
				monster->tile_y, monster->hp);
			monster->hp -= 20;
			monster->is_dirty = 1;
			py_update_monster_from_c(monster);

			if (monster->hp <= 0) {
				PyObject *tmp;
				tmp = PyObject_CallMethod(monster->py_obj, "isDead", NULL);
				if (tmp != NULL)
					Py_DECREF(tmp);

				if (monster->prev != NULL)
					monster->prev->next = monster->next;
				if (monster->next != NULL)
					monster->next->prev = monster->prev;
				if (global_GS.current_map->monsters == monster)
					global_GS.current_map->monsters = (adv_monster*)monster->next;
				
				Py_DECREF(monster->py_obj);
				free(monster);
				break;
			}

			PyObject *tmp;
			tmp = PyObject_CallMethod(monster->py_obj, "isHit", NULL);
			if (tmp != NULL)
				Py_DECREF(tmp);
			break;
		}
	}
#endif

	return 0;
}
