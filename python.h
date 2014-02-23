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
int py_update_base_object(adv_base_object *obj);
int py_update_object_timer(adv_base_object *obj);

int py_update_monster(adv_monster *monster);
adv_monster *py_new_monster_from_object(PyObject *obj);

int setup_python(int argc, char *argv[]);
adv_tile *py_get_tile(PyObject *py_obj);

#endif /* end of include guard: ADV_PYTHON_H */
