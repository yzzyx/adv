#include <Python.h>
#include "common.h"
#include "animation.h"
#include "player.h"
#include "map.h"
#include "python.h"
#include "astar.h"
#include "object.h"

PyObject *main_module;
PyObject *main_dict;

PyObject *attribute_list[ATTR_MAX];

void
py_CallMethodNoArgs(PyObject *obj, char *method)
{
	PyObject *tmp;

	if (!PyObject_HasAttrString(obj,method))
		return;
	tmp = PyObject_CallMethod(obj, method, NULL);
	if (tmp)
		Py_DECREF(tmp);
}

/*
 * py_animLoadSpritesheet(): [animLoadSpritesheet(filename, useAlphaBlend = 1)]
 *
 * Load a spritesheet, with alpha if specified
 */
PyObject *
py_animLoadSpritesheet(PyObject *self, PyObject *args)
{
	const char *filename;
	int ss_id = 0;
	int use_alpha = 1;

	if (!PyArg_ParseTuple(args, "s|i", &filename, &use_alpha)) {
		printf("py_loadSpritesheet():\n");
		PyErr_Print();
		return NULL;
	}

	ss_id = animation_load_spritesheet(filename);
	if (use_alpha)
		animation_set_spritesheet_blendmode(ss_id, SDL_BLENDMODE_BLEND);

	PyObject *py_ss = Py_BuildValue("i", ss_id);
	return py_ss;
}

/*
 * py_animCreate(): [animCreate(spritesheet_id, start_sprite, length)]
 *
 * Create an animation from a given spritesheet
 */
PyObject *
py_animCreate(PyObject *self, PyObject *args)
{
	int ss_id;
	int start;
	int length;
	int anim_id;

	if (!PyArg_ParseTuple(args, "iii", &ss_id, &start, &length)) {
		printf("py_animCreate():\n");
		PyErr_Print();
		return NULL;
	}

	anim_id = animation_create(ss_id, start, length);

	PyObject *py_ss = Py_BuildValue("i", anim_id);
	return py_ss;
}

/*
 * py_getPlayer(): [getPlayer()]
 *
 * Returns the player object
 */
PyObject *
py_getPlayer(PyObject *self, PyObject *args)
{

	Py_INCREF(main_player);
	return main_player;
}

/*
 * py_getDistance(): [getDistance(x1,y1,x2,y2)]
 *
 * Returns distance between two points
 */
PyObject *
py_getDistance(PyObject *self, PyObject *args)
{
	int x1, y1, x2, y2;

	if (!PyArg_ParseTuple(args, "iiii", &x1, &y1, &x2, &y2)) {
		printf("py_getDistance():\n");
		PyErr_Print();
		return NULL;
	}

	int x, y;
	x = abs(x1-x2);
	y = abs(y1-y2);
	PyObject *py_distance = Py_BuildValue("f", sqrtf(x*x + y*y));

	return py_distance;
}

/*
 * py_getPath(): [getPath(monster, x1, y1)]
 *
 * Called from python, returns walking path from the monsters current position
 * to (x1,y1). Return value is a string, which can contain 4 different
 * characters - N,E,S,W
 */
PyObject *
py_getPath(PyObject *self, PyObject *args)
{
	PyObject *monster;
	int x1, y1;

	if (!PyArg_ParseTuple(args, "Oii", &monster, &x1, &y1)) {
		printf("py_getPath():\n");
		PyErr_Print();
		return NULL;
	}

	/* FIXME, pathfinder() only returns next direction as of now
	   adv_monster *m;
	 char *str;
	m = monster_get_from_pyobj(monster);
	 str = pathfinder(m, m->tile_x, m->tile_y, &x1, &y1);
	 PyObject *py_str = PyString_FromString(str);
	 free(str);
	 */

	PyObject *py_str = PyString_FromString("NESW");
	return py_str;
}

/*
 * py_isVisible(): [isVisible(monster, x, y)]
 *
 * Returns true if position (x,y) is visible for monster, otherwise false
 *
 */
PyObject *
py_isVisible(PyObject *self, PyObject *args)
{
	PyObject *monster;
	int x1, y1;

	if (!PyArg_ParseTuple(args, "Oii", &monster, &x1, &y1)) {
		printf("py_getPath():\n");
		PyErr_Print();
		return NULL;
	}


	int isvisible;

	isvisible = monster_position_is_visible(monster, x1, y1);
	return PyBool_FromLong(isvisible);
}

/*
 * py_monster_gotoPosition(): [monster_gotoPosition(monster, x, y)]
 *
 * Monster will start walking towards (x,y)
 *
 */
PyObject *
py_monster_gotoPosition(PyObject *self, PyObject *args)
{
	PyObject *monster;
	int x1, y1;

	if (!PyArg_ParseTuple(args, "Oii", &monster, &x1, &y1)) {
		printf("py_monster_gotoPosition():\n");
		PyErr_Print();
		return NULL;
	}


	monster_goto_position(monster, x1, y1);
	return PyBool_FromLong(1);
}

/*
 * py_monster_gotoDirection(): [monster_gotoDirection(monster, dir)]
 *
 * Monster will start walking in direction dir
 *
 */
PyObject *
py_monster_gotoDirection(PyObject *self, PyObject *args)
{
	PyObject *monster;
	int dir;

	if (!PyArg_ParseTuple(args, "Oi", &monster, &dir)) {
		printf("py_monster_gotoDirection():\n");
		PyErr_Print();
		return NULL;
	}

	int ret;

	ret = monster_goto_direction(monster, dir);
	return PyBool_FromLong(ret);
}

/*
 * py_monster_attack(): [monster_attack(monster, x, y)]
 *
 * Monster should attack tile x,y
 */
PyObject *
py_monster_attack(PyObject *self, PyObject *args)
{
	PyObject *monster;
	int x, y;

	if (!PyArg_ParseTuple(args, "Oii", &monster, &x, &y)) {
		printf("py_monster_attack():\n");
		PyErr_Print();
		return NULL;
	}

	monster_attack(monster, x, y);
	return PyBool_FromLong(1);
}

static PyMethodDef methods[] = {
    {"loadSpritesheet",		py_animLoadSpritesheet, METH_VARARGS, "load spritesheet from file" },
    {"createAnimation",		py_animCreate,		METH_VARARGS, "create animation from spritesheet" },
    {"getPlayer",		py_getPlayer,		METH_VARARGS, "get the player object" },
    {"getDistance",		py_getDistance,		METH_VARARGS, "getDistace(x1,y2,x2,y2): get absolute distance from (x1,y1) to (x2,y2)" },
    {"getPath",			py_getPath,		METH_VARARGS, "getPath(monster,x2,y2): get walking path for monster to (x2,y2)" },
    {"isVisible",		py_isVisible,		METH_VARARGS, "isVisible(monster, x1, y1): Check if monster can see position x1,y1" },
    {"monster_gotoPosition",	py_monster_gotoPosition,METH_VARARGS, "monster_gotoPosition(monster, x1, y1): monster should start walking towards (x,y) (if possible)" },
    {"monster_gotoDirection",	py_monster_gotoDirection,METH_VARARGS, "monster_gotoDirection(monster, direction): monster should start walking in direction (if possible)" },
	{"monster_attack",		py_monster_attack, METH_VARARGS, "monster_attack(monster, x, y): attack tile @ x,y" },
//    {"monster_gotoPosition", monster_gotoPosition, METH_VARARGS, "monster_gotoPosition" },
    {NULL, NULL, 0, NULL}
};


long
py_get_int(PyObject *obj)
{
	long intval;

	if (obj == NULL) {
		PyErr_Print();
		return 0;
	}
	
	if (obj == Py_None)
		return 0;

	intval = PyInt_AsLong(obj);
	return intval;
}

long
py_get_int_decref(PyObject *obj)
{
	long intval;
	if (obj == NULL) {
		PyErr_Print();
		return 0;
	}
	
	if (obj == Py_None)
		return 0;

	intval = PyInt_AsLong(obj);
	Py_DECREF(obj);
	return intval;
}

adv_tile *
py_get_tile(PyObject *py_obj)
{
	adv_tile *t;

	t = malloc(sizeof *t);
	t->py_obj = py_obj;

	/*
	PyObject *sprite_animation;
	sprite_animation = PyObject_GetAttrString(py_obj, "sprite_animation");
	if (sprite_animation == NULL || sprite_animation == Py_None) {
		t->animation_id = -1;
		t->animation_frame = 0;
		PyErr_Print();
	} else {
		t->animation_id = py_get_int(PyDict_GetItemString(sprite_animation, "id"));
		t->animation_frame = py_get_int(PyDict_GetItemString(sprite_animation, "current_frame"));
	}
	*/
	t->spritesheet = py_get_int_decref(PyObject_GetAttrString(py_obj, "spritesheet"));
	t->spriteid = py_get_int_decref(PyObject_GetAttrString(py_obj, "spriteid"));
	t->walkable = py_get_int_decref(PyObject_GetAttrString(py_obj, "walkable"));
	t->visibility = py_get_int_decref(PyObject_GetAttrString(py_obj, "visibility"));
	return t;
}

/*
 * py_update_object_timer(obj)
 *
 * Called on each tick for an object
 * If the object has a timeout set, it will call the "tick()"-function
 * on the python object. Otherwise it will do nothing
 */
int
py_update_object_timer(PyObject *obj)
{
	long timer;

	timer = py_getattr_int(obj, ATTR_TIMER);
	if (timer == -1)
		return 0;

	timer --;
	if (timer == 0) {
		PyObject *tmp;
		
		tmp = PyObject_CallMethod(obj, "tick", NULL);
		if (tmp == NULL) {
			printf("PyObject_CallMethod(tick):");
			PyErr_Print();
		} else {
			Py_DECREF(tmp);
		}

		return 1;
	}

	/* Update time-counter */
	py_setattr_int(obj, ATTR_TIMER, timer);

	return 0;
}


int
setup_python(int argc, char *argv[])
{
	PyObject *adv_module;

	Py_SetProgramName(argv[0]);
#ifdef WIN32
	Py_SetPythonHome("./python");
#endif

	Py_Initialize();
	
	Py_InitModule("_adv", methods);
	adv_module = PyImport_ImportModule("_adv");
	if (adv_module == NULL) { PyErr_Print(); return -1; }

	main_module = PyImport_AddModule("__main__");
	main_dict = PyModule_GetDict(main_module);
	if (main_module == NULL || main_dict == NULL) { PyErr_Print(); return -1; }

	PyObject *PyFileObject = PyFile_FromString("load.py", "r");
	PyObject *p = PyRun_File(PyFile_AsFile(PyFileObject), "load.py", Py_file_input,
	    main_dict, main_dict);
	Py_DECREF(PyFileObject);

	if (p == NULL) {
		PyErr_Print();
		return -1;
	}

	/* Setup attribute objects */
	attribute_list[ATTR_TIMER] = PyString_FromString("timer");
	attribute_list[ATTR_HAS_DIRECTIONS] = PyString_FromString("has_directions");
	attribute_list[ATTR_IS_DIRTY] = PyString_FromString("is_dirty");
	attribute_list[ATTR_X] = PyString_FromString("x");
	attribute_list[ATTR_Y] = PyString_FromString("y");
	attribute_list[ATTR_ANIMATION] = PyString_FromString("animation");
	attribute_list[ATTR_ANIMATION_STOPPED] =
		PyString_FromString("animation_stopped");
	attribute_list[ATTR_ANIMATION_MOVING] =
		PyString_FromString("animation_moving");
	attribute_list[ATTR_HP] = PyString_FromString("hp");
	attribute_list[ATTR_MP] = PyString_FromString("mp");
	attribute_list[ATTR_IN_MOVEMENT] = PyString_FromString("int_in_movement");
	attribute_list[ATTR_DRAW_MOVEMENT] = PyString_FromString("int_draw_movement");
	attribute_list[ATTR_DIRECTION] = PyString_FromString("direction");

	attribute_list[ATTR_INT_SPEED] = PyString_FromString("int_speed");
	attribute_list[ATTR_INT_TARGET_X] =
		PyString_FromString("int_target_x");
	attribute_list[ATTR_INT_TARGET_Y] =
		PyString_FromString("int_target_y");
	attribute_list[ATTR_INT_ATTACK_TARGET_X] =
		PyString_FromString("int_attack_target_x");
	attribute_list[ATTR_INT_ATTACK_TARGET_Y] =
		PyString_FromString("int_attack_target_y");
	attribute_list[ATTR_INT_ACTIVE_PATH] =
		PyString_FromString("int_active_path");

	Py_DECREF(p);
	return 0;
}

long
py_getattr_int(PyObject *obj, attribute_enum attr)
{
	PyObject *py_int;
	long c_int = -1;

	py_int = PyObject_GetAttr(obj, attribute_list[attr]);
	if (!py_int || py_int == Py_None) {
		PyErr_Print();
	} else {
		c_int = PyInt_AsLong(py_int);
		Py_DECREF(py_int);
	}
	return c_int;
}

long
py_getattr_list_int(PyObject *obj, attribute_enum attr, int item)
{
	PyObject *py_list;
	PyObject *py_int;
	long c_int = 0;

	py_list = PyObject_GetAttr(obj, attribute_list[attr]);
	py_int = PyList_GetItem(py_list, item);
	if (py_int == NULL) {
		printf("py_getattr_list_int(): \n");
		PyErr_Print();
	} else {
		c_int = PyInt_AsLong(py_int);
	}
	Py_DECREF(py_list);

	return c_int;
}

int
py_setattr_int(PyObject *obj, attribute_enum attr, long val)
{
	PyObject *py_int;

	py_int = PyInt_FromLong(val);
	if (PyObject_SetAttr(obj, attribute_list[attr], py_int) == -1) {
		printf("py_setattr_int():\n");
		PyErr_Print();
		return -1;
	}

	return 0;
}
