#ifndef PLAYER_H
#define PLAYER_H
#include "common.h"
#include "object.h"

PyObject *setup_player();
int monster_move(PyObject *p);

int monster_goto_direction(PyObject *p, int direction);
int monster_goto_position(PyObject *p, int x, int y);

int monster_position_is_visible(PyObject *m, int x, int y);

extern PyObject *main_player;

int monster_attack(PyObject *m, int x, int y);

#endif /* end of include guard: PLAYER_H */

