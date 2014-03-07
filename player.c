#include <Python.h>
#include "animation.h"
#include "sdl.h"
#include "player.h"
#include "common.h"
#include "astar.h"
#include "map.h"
#include "python.h"
#include "gamestate.h"

#define TICK_LENGTH 3

adv_monster *
setup_player()
{
	adv_monster *p;

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

	PyObject *obj_inst = PyObject_CallObject(obj_def, NULL);
	if (obj_inst == NULL) {
		printf("player::PyObject_CallObject():\n");
		PyErr_Print();
		return NULL;
	}

	p = py_new_monster_from_pyobj(obj_inst);
	return p;
}

/* collision_player(player, map)
 *
 * Check for collision
 */
int
player_map_is_walkable(adv_monster *p)
{
	int tx, ty;
	int min_x, min_y, max_x, max_y;

	min_x = p->xx / SPRITE_SIZE;
	min_y = p->yy / SPRITE_SIZE;

	max_x = min_x + (((p->xx % SPRITE_SIZE) > 0) ? 1:0);
	max_y = min_y + (((p->yy % SPRITE_SIZE) > 0) ? 1:0);
	if (max_x >= global_GS.current_map->width)
		max_x = global_GS.current_map->width - 1;
	if (max_y >= global_GS.current_map->height)
		max_y = global_GS.current_map->height - 1;

	for (tx = min_x; tx <= max_x; tx ++) {
		for (ty = min_y; ty <= max_y; ty ++) {
			if (!global_GS.current_map->tiles[tx+ty*global_GS.current_map->width]->walkable)
				return 0;
		}
	}
			
	return 1;

}

int monster_move_direction_int(adv_monster *p, int direction)
{
	int mx = 0, my = 0;

	/* Assume we will not move */
	p->in_movement = 0;

	switch(direction) {
		case DIRECTION_UP: mx = 0; my = -1; break;
		case DIRECTION_DOWN: mx = 0; my = 1; break;
		case DIRECTION_LEFT: mx = -1; my = 0; break;
		case DIRECTION_RIGHT: mx = 1; my = 0; break;
		default:
			return 0;
			break;
	}

	/* Don't even try to walk where we can't */
	if (p->mod_x == 0 && p->mod_y == 0 &&
	    !map_is_walkable(p, p->tile_x + mx, p->tile_y + my)) {
		return 0;
	}

	p->direction = direction;
	p->in_movement = 1;
	p->draw_movement = 1;

	int i;
	for (i=0; i < p->speed; i ++) {
		p->mod_x += mx;
		p->mod_y += my;

		if (p->mod_x == mx * (SPRITE_SIZE>>1) &&
		    p->mod_y == my * (SPRITE_SIZE>>1)) {
			p->tile_x += mx;
			p->tile_y += my;
			p->is_dirty = 1;
			p->mod_x = -p->mod_x;
			p->mod_y = -p->mod_y;
		}

		if (p->mod_x == 0  && p->mod_y == 0) {
			p->in_movement = 0;
			if (p->queued_target_x != -1) {
				p->target_tile_x = p->queued_target_x;
				p->queued_target_x = -1;
			}
			if (p->queued_target_y != -1) {
				p->target_tile_y = p->queued_target_y;
				p->queued_target_y = -1;
			}
			break;
		}
	}

	p->xx = p->tile_x * SPRITE_SIZE + p->mod_x;
	p->yy = p->tile_y * SPRITE_SIZE + p->mod_y;

	return 1;
}

/* move_player
 *
 * called each tick, updating the player's position
 */
int monster_move(adv_monster *p)
{
	int dir;
	int x2, y2;

	/* check for errors */
	if (p->target_tile_x < 0) p->target_tile_x = 0;
	if (p->target_tile_y < 0) p->target_tile_y = 0;
	if (p->target_tile_x > global_GS.current_map->width - 1)
		p->target_tile_x = global_GS.current_map->width - 1;
	if (p->target_tile_y > global_GS.current_map->height - 1)
		p->target_tile_y = global_GS.current_map->height - 1;

	if (p->mod_x == 0 && p->mod_y == 0 &&
	    p->target_tile_x == p->tile_x && p->target_tile_y == p->tile_y)
		return 0;

	if (p->mod_x != 0 || p->mod_y != 0)
		return monster_move_direction_int(p, p->direction);

	if (p->target_tile_x == p->tile_x - 1 && p->target_tile_y == p->tile_y) return monster_move_direction_int(p, DIRECTION_LEFT);
	if (p->target_tile_x == p->tile_x + 1 && p->target_tile_y == p->tile_y) return monster_move_direction_int(p, DIRECTION_RIGHT);
	if (p->target_tile_x == p->tile_x && p->target_tile_y == p->tile_y - 1) return monster_move_direction_int(p, DIRECTION_UP);
	if (p->target_tile_x == p->tile_x && p->target_tile_y == p->tile_y + 1) return monster_move_direction_int(p, DIRECTION_DOWN);

	x2 = p->target_tile_x;
	y2 = p->target_tile_y;
	dir = pathfinder(p, p->tile_x, p->tile_y, &x2, &y2);
	return monster_move_direction_int(p, dir);

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
monster_goto_direction(adv_monster *p, int direction)
{
	int mx, my;
	int target_x, target_y;

	switch(direction) {
		case DIRECTION_UP: mx = 0; my = -1; break;
		case DIRECTION_DOWN: mx = 0; my = 1; break;
		case DIRECTION_LEFT: mx = -1; my = 0; break;
		case DIRECTION_RIGHT: mx = 1; my = 0; break;
		default:
			return 0;
			break;
	}

	target_x = p->tile_x + mx;
	target_y = p->tile_y + my;
	if (target_x < 0) target_x = 0;
	if (target_y < 0) target_y = 0;
	if (target_x >= global_GS.current_map->width)
		target_x = global_GS.current_map->width - 1;
	if (target_y >= global_GS.current_map->height)
		target_y = global_GS.current_map->height - 1;

	/* Don't even try to walk where we can't */
	if (!map_is_walkable(p, target_x, target_y))
		return 0;

	if (p->in_movement) {
		p->queued_target_x = target_x;
		p->queued_target_y = target_y;
	} else {
		p->target_tile_x = target_x;
		p->target_tile_y = target_y;
	}
	p->is_dirty = 1;

	return 1;
}


/*
 * monster_goto_position(m, x, y)
 *
 * Try to make monster walk to x, y
 * Returns 1 if it's possible, or 0 otherwise
 */
int
monster_goto_position(adv_monster *p, int x, int y)
{
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= global_GS.current_map->width)
		x = global_GS.current_map->width - 1;
	if (y >= global_GS.current_map->height)
		y = global_GS.current_map->height - 1;

	if (p->in_movement) {
		p->queued_target_x = x;
		p->queued_target_y = y;
	} else {
		p->target_tile_x = x;
		p->target_tile_y = y;
	}
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

/*
 * monster_attack(m, x, y)
 *
 * Attack in direction of (x,y)
 * The actual outcome differs depending on weapons, etc.
 */
int
monster_attack(adv_monster *m, int x, int y)
{

	int dir;
	int mx = 0, my = 0;
	float angle = atan2f(x - m->tile_x,
						 y - m->tile_y) * 180/M_PI;

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

	animation_play(rs.attack_animations[dir], m->tile_x + mx, m->tile_y + my);

	/* Check if player is standing there */
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

	return 0;
}
