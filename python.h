#ifndef ADV_PYTHON_H
#define ADV_PYTHON_H
#include <Python.h>
#include "map.h"

typedef struct t_adv_tile adv_tile;
extern PyObject *main_dict;

int py_get_int(PyObject *obj);
int py_get_int_decref(PyObject *obj);
int setup_python(int argc, char *argv[]);
adv_tile *py_get_tile(PyObject *py_obj);

#endif /* end of include guard: ADV_PYTHON_H */

