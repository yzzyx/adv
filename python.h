#ifndef ADV_PYTHON_H
#define ADV_PYTHON_H
#include <Python.h>
#include "map.h"

typedef struct t_adv_map adv_map;
extern PyObject *main_dict;

int py_get_int(PyObject *obj);
int setup_python(int argc, char *argv[]);
adv_map *python_generate_map(const char *map_name);

#endif /* end of include guard: ADV_PYTHON_H */

