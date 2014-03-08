#ifndef OBJECT_H
#define OBJECT_H
#include "common.h"

typedef struct t_adv_object {
	ADV_OBJECT_HEADER
	ADV_OBJECT_LIST

	int xx, yy; /* The absolute position of the object */
	int tile_x; /* Position on map */
	int tile_y;
	int type;

	int animation;
}adv_object;

void object_list_free(adv_base_object *list);
adv_base_object *object_list_find_pyobj(adv_base_object *list, PyObject *py_obj);
#endif /* end of include guard: OBJECT_H */
