#ifndef ADV_PYTHON_H
#define ADV_PYTHON_H
#include <Python.h>
#include "player.h"
#include "map.h"

typedef struct t_adv_tile adv_tile;
extern PyObject *main_dict;

int py_get_int(PyObject *obj);
int py_get_int_decref(PyObject *obj);
int pyobj_is_dirty(PyObject *obj);
int py_update_object_timer(PyObject *obj);

int setup_python(int argc, char *argv[]);
adv_tile *py_get_tile(PyObject *py_obj);

typedef enum{
	ATTR_TIMER,
	ATTR_HAS_DIRECTIONS,
	ATTR_IS_DIRTY,
	ATTR_X,
	ATTR_Y,
	ATTR_ANIMATION,
	ATTR_ANIMATION_STOPPED,
	ATTR_ANIMATION_MOVING,
	ATTR_HP,
	ATTR_MP,
	ATTR_IN_MOVEMENT,
	ATTR_DRAW_MOVEMENT,
	ATTR_DIRECTION,

	/* Used by engine */
	ATTR_INT_SPEED,
	ATTR_INT_X,
	ATTR_INT_Y,
	ATTR_INT_MOD_X,
	ATTR_INT_MOD_Y,
	ATTR_INT_TARGET_TILE_X,
	ATTR_INT_TARGET_TILE_Y,
	ATTR_INT_ATTACK_TARGET_X,
	ATTR_INT_ATTACK_TARGET_Y,

	ATTR_MAX
}attribute_enum;

int py_getattr_int(PyObject *obj, attribute_enum attr);
int py_getattr_list_int(PyObject *obj, attribute_enum attr, int item);
int py_setattr_int(PyObject *obj, attribute_enum attr, int val);

#endif /* end of include guard: ADV_PYTHON_H */
