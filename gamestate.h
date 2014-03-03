#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "python.h"

typedef struct t_gamestate {
	PyObject *py_obj;
	adv_map *current_map;
	int is_dirty;
}gamestate;

extern gamestate global_GS;

int gamestate_update();
int gamestate_init();
#endif /* end of include guard: GAMESTATE_H */
