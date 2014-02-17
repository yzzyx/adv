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
		p->animation_id = py_get_int(PyDict_GetItemString(sprite_animation, "__id__"));
		p->animation_frame = py_get_int(PyDict_GetItemString(sprite_animation, "current_frame"));
	}

	p->hp = py_get_int(PyObject_GetAttrString(p->py_obj, "hp"));
	p->mp = py_get_int(PyObject_GetAttrString(p->py_obj, "mp"));
	p->tile_x = py_get_int(PyObject_GetAttrString(p->py_obj, "x"));
	p->tile_y = py_get_int(PyObject_GetAttrString(p->py_obj, "y"));
	p->speed = py_get_int(PyObject_GetAttrString(p->py_obj, "speed"));
	return p;


}

player *
setup_player()
{
	player *p;

	p = malloc(sizeof *p);
	memset(p, 0, sizeof *p);

	PyObject *module = PyDict_GetItemString(main_dict, "player");
	if (module == NULL) {
		PyErr_Print();
		free(p);
		return NULL;
	}
	PyObject *obj_def = PyObject_GetAttrString(module, "Player");
	if (obj_def == NULL) {
		PyErr_Print();
		return NULL;
	}

	PyObject *obj_inst = PyObject_CallObject(obj_def, NULL);
	if (obj_inst == NULL) {
		PyErr_Print();
		return NULL;
	}

	p->py_obj = obj_inst;
	py_update_c_player(p);
	return p;
}

/* move_player
 *
 * called each tick, updating the player's position
 */
int move_player(adv_map *m, player *p)
{
	int new_tile = 0;
	int prev_x, prev_y;

	prev_x = p->tile_x;
	prev_y = p->tile_y;
	if (p->movement_x != 0) {
		p->xx += p->movement_x * p->speed;
		if (abs(p->xx) >= FRAME_WIDTH) {
		 	p->tile_x += (p->xx > 0) ? 1:-1;
			p->xx = 0;
			p->movement_x = 0;
			new_tile = 1;
		}
	}

	if (p->movement_y != 0) {
		p->yy += p->movement_y * p->speed;
		if (abs(p->yy) >= FRAME_WIDTH) {
			p->tile_y += (p->yy > 0) ? 1:-1;
			p->yy = 0;
			p->movement_y = 0;
			new_tile = 1;
		}
	}

	if (new_tile) {
		/* Call playerEnter-method on tile object */
		PyObject *tmp;
		tmp = PyObject_CallMethod(m->tiles[prev_x + prev_y * m->width]->py_obj,
		    "playerExit", "", NULL);
		if (tmp == NULL) { PyErr_Print(); return -1; }
		Py_DECREF(tmp);

		tmp = PyObject_CallMethod(m->tiles[p->tile_x + p->tile_y * m->width]->py_obj,
		    "playerEnter", "", NULL);
		if (tmp == NULL) { PyErr_Print(); return -1; }
		Py_DECREF(tmp);
	}

	return 0;
}
