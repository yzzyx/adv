#include <Python.h>
#include "animation.h"
#include "player.h"
#include "common.h"
#include "map.h"

#define TICK_LENGTH 3

/*
 * py_update_c_player()
 *
 * Copy information from python to C
 */
int
py_update_c_player(player *p)
{
	PyObject *sprite_animation;
	sprite_animation = PyObject_GetAttrString(p->py_obj, "sprite_animation");
	if (sprite_animation == NULL) {
		p->animation_id = -1;
		PyErr_Print();
	} else {
		p->animation_id = py_get_int(PyDict_GetItemString(sprite_animation, "id"));
		p->animation_frame = py_get_int(PyDict_GetItemString(sprite_animation, "current_frame"));
	}

	p->hp = py_get_int(PyObject_GetAttrString(p->py_obj, "hp"));
	p->mp = py_get_int(PyObject_GetAttrString(p->py_obj, "mp"));
	p->tile_x = py_get_int(PyObject_GetAttrString(p->py_obj, "x"));
	p->tile_y = py_get_int(PyObject_GetAttrString(p->py_obj, "y"));
	p->speed = py_get_int(PyObject_GetAttrString(p->py_obj, "speed"));
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
player_map_is_walkable(player *p, adv_map *m)
{
	int tx, ty;
	int min_x, min_y, max_x, max_y;

	min_x = p->xx / FRAME_WIDTH;
	min_y = p->yy / FRAME_WIDTH;

	max_x = min_x + (((p->xx % FRAME_WIDTH) > 0) ? 1:0);
	max_y = min_y + (((p->yy % FRAME_WIDTH) > 0) ? 1:0);
	if (max_x >= m->width) max_x = m->width - 1;
	if (max_y >= m->height) max_y = m->height - 1;

	for (tx = min_x; tx <= max_x; tx ++) {
		for (ty = min_y; ty <= max_y; ty ++) {
			if (!m->tiles[tx+ty*m->width]->walkable)
				return 0;
		}
	}
			
	return 1;

}

/* move_player
 *
 * called each tick, updating the player's position
 */
int move_player(adv_map *m, player *p)
{
	int new_tile = 0;
	int prev_x, prev_y;
	int prev_tile_x, prev_tile_y;
	static int last_tick = 0;

	prev_x = p->xx;
	prev_y = p->yy;
	prev_tile_x = p->tile_x;
	prev_tile_y = p->tile_y;


	if (p->movement_x == 0 && p->movement_y == 0)
		return 0;

	move_player_cnt ++;
	p->in_movement = 1;

	int i;
	for (i = 0; i < p->speed; i++) {
		//	if ((SDL_GetTicks() - last_tick) > (unsigned int)p->speed) {
		//		last_tick = SDL_GetTicks();
		p->xx += p->movement_x;// * p->speed;
		p->yy += p->movement_y;// * p->speed;
		//	}


		if (p->xx < 0) p->xx = 0;
		else if (p->xx > (m->width-1)*FRAME_WIDTH)
			p->xx = (m->width-1)*FRAME_WIDTH;

		if (p->yy < 0) p->yy = 0;
		else if (p->yy > (m->height-1)*FRAME_HEIGHT)
			p->yy = (m->height-1)*FRAME_HEIGHT;

		if (!player_map_is_walkable(p, m)) {
			p->xx = prev_x;
			p->yy = prev_y;
			p->movement_x = 0;
			p->movement_y = 0;
			p->in_movement = 0;
			return 0;
		}

		p->tile_x = p->xx / FRAME_WIDTH;
		p->tile_y = p->yy / FRAME_HEIGHT;

		if(p->xx % FRAME_WIDTH == 0 &&
		    p->yy % FRAME_HEIGHT == 0) {
			p->movement_x = 0;
			p->movement_y = 0;
			p->in_movement = 0;
			new_tile = 1;
			break;
		}
	}


	if (new_tile) {
		/* Call playerExit-method on tile we're leaving*/
		PyObject *tmp;
		tmp = PyObject_CallMethod(
		    m->tiles[prev_tile_x + prev_tile_y * m->width]->py_obj,
		    "playerExit", "", NULL);
		if (tmp == NULL) { PyErr_Print(); return -1; }
		Py_DECREF(tmp);

		/* Call playerEnter-method on tile we're entering*/
		tmp = PyObject_CallMethod(m->tiles[p->tile_x + p->tile_y * m->width]->py_obj,
		    "playerEnter", "", NULL);
		if (tmp == NULL) { PyErr_Print(); return -1; }
		Py_DECREF(tmp);
	}

	return 1;
}
