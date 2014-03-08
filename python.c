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

	Py_INCREF(main_player->py_obj);
	return main_player->py_obj;
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


	adv_monster *m;
	int isvisible;

	m = monster_get_from_pyobj(monster);
	isvisible = monster_position_is_visible(m, x1, y1);
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

	adv_monster *m;

	m = monster_get_from_pyobj(monster);
	monster_goto_position(m, x1, y1);
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

	adv_monster *m;
	int ret;

	m = monster_get_from_pyobj(monster);
	ret = monster_goto_direction(m, dir);
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

	adv_monster *m;
	m = monster_get_from_pyobj(monster);
	monster_attack(m, x, y);
	return PyBool_FromLong(1);
}

int
pyobj_is_dirty(PyObject *obj)
{
	return py_get_int_decref(PyObject_GetAttrString(obj, "is_dirty"));
}

adv_base_object *
py_update_list(adv_base_object *c_list, PyObject *py_obj, const char *python_attribute)
{
	adv_base_object *obj;

	PyObject *list;
	list = PyObject_GetAttrString(py_obj, python_attribute);
	if (list == NULL) {
		printf("py_update_list:GetAttrString():");
		PyErr_Print();
		object_list_free(c_list);
		return NULL;
	}

	if (list == Py_None)
		return NULL;

	int x;
	for (x = 0; x < PyList_Size(list); x ++) {
		py_obj = PyList_GetItem(list, x);
		if (py_obj == Py_None)
			continue;

		/* Check if this object is already in our list */
		obj = object_list_find_pyobj(c_list, py_obj);
		if (obj == NULL) {
			/* Add object */
			obj = (adv_base_object*)py_new_object_from_pyobj(py_obj);
			if (c_list != NULL) {
				obj->next = c_list;
				c_list->prev = obj;
			}
		} else if (pyobj_is_dirty(py_obj)) {
			printf("FIXME - update object\n");
		}
	}

	return obj;
}

int 
py_update_base_object(adv_base_object *obj)
{
	obj->timer = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "timer"));
	obj->has_directions = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "has_directions"));

	return 0;
}

int
py_update_monster(adv_monster *monster)
{
	/*
	PyObject *sprite_animation;
	sprite_animation = PyObject_GetAttrString(monster->py_obj, "sprite_animation");
	if (sprite_animation == NULL) {
		monster->animation_id = -1;
		PyErr_Print();
		return -1;
	} else {
		monster->animation_id = py_get_int(PyDict_GetItemString(sprite_animation, "id"));
		monster->animation_frame = py_get_int(PyDict_GetItemString(sprite_animation, "current_frame"));
	}
	*/
	py_update_base_object((adv_base_object*)monster);

	monster->spritesheet = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "spritesheet"));

	PyObject *list;
	list = PyObject_GetAttrString(monster->py_obj, "animation_stopped");
	if (list == NULL) {
		PyErr_Print();
		monster->animation_stopped[0] = -1;
		monster->animation_stopped[1] = -1;
		monster->animation_stopped[2] = -1;
		monster->animation_stopped[3] = -1;
	} else {
		monster->animation_stopped[0] = py_get_int(PyList_GetItem(list, 0));
		if (monster->has_directions) {
			monster->animation_stopped[1] = py_get_int(PyList_GetItem(list, 1));
			monster->animation_stopped[2] = py_get_int(PyList_GetItem(list, 2));
			monster->animation_stopped[3] = py_get_int(PyList_GetItem(list, 3));
		}
	}
	
	list = PyObject_GetAttrString(monster->py_obj, "animation_moving");
	if (list == NULL) {
		PyErr_Print();
		monster->animation_moving[0] = -1;
		monster->animation_moving[1] = -1;
		monster->animation_moving[2] = -1;
		monster->animation_moving[3] = -1;
	} else {
		monster->animation_moving[0] = py_get_int(PyList_GetItem(list, 0));
		if (monster->has_directions) {
			monster->animation_moving[1] = py_get_int(PyList_GetItem(list, 1));
			monster->animation_moving[2] = py_get_int(PyList_GetItem(list, 2));
			monster->animation_moving[3] = py_get_int(PyList_GetItem(list, 3));
		}
	}

	monster->hp = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "hp"));
	monster->mp = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "mp"));
	monster->tile_x = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "x"));
	monster->tile_y = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "y"));

/*
	if (monster->xx % SPRITE_SIZE != monster->tile_x)
		monster->xx = monster->tile_x * SPRITE_SIZE;
	if (monster->yy % SPRITE_SIZE != monster->tile_x)
		monster->yy = monster->tile_y * SPRITE_SIZE;
*/
//	monster->target_tile_x = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "target_x"));
//	monster->target_tile_y = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "target_y"));
	PyObject *tmp;

	tmp = PyObject_CallMethod(monster->py_obj, "getMovementSpeed", NULL);
	if (tmp) {
		monster->speed = py_get_int_decref(tmp);
	}

	tmp = PyObject_CallMethod(monster->py_obj, "getAttackSpeed", NULL);
	if (tmp) {
		monster->attack_speed = py_get_int_decref(tmp);
	}

	/* Update inventory */
	monster->inventory = (adv_object*)py_update_list(
		(adv_base_object*)monster->inventory, monster->py_obj, "inventory");

	PyObject_SetAttrString(monster->py_obj, "is_dirty", Py_BuildValue("i", 0));
	return 0;
}

int
py_update_monster_from_c(adv_monster *monster)
{
	PyObject_SetAttrString(monster->py_obj, "hp", Py_BuildValue("i", monster->hp));
	PyObject_SetAttrString(monster->py_obj, "mp", Py_BuildValue("i", monster->mp));
	PyObject_SetAttrString(monster->py_obj, "x", Py_BuildValue("i", monster->tile_x));
	PyObject_SetAttrString(monster->py_obj, "y", Py_BuildValue("i", monster->tile_y));

	PyObject_SetAttrString(monster->py_obj, "target_x", Py_BuildValue("i", monster->target_tile_x));
	PyObject_SetAttrString(monster->py_obj, "target_y", Py_BuildValue("i", monster->target_tile_y));
	PyObject_SetAttrString(monster->py_obj, "speed", Py_BuildValue("i", monster->speed));
	monster->is_dirty = 0;
	return 0;
}

adv_monster *
py_new_monster_from_pyobj(PyObject *obj)
{
	adv_monster *monster;

	monster = malloc(sizeof *monster);
	memset(monster, 0, sizeof *monster);
	monster->py_obj = obj;
	py_update_base_object((adv_base_object*)monster);
	py_update_monster(monster);

	monster->target_tile_x = monster->tile_x;
	monster->target_tile_y = monster->tile_y;
	monster->queued_target_x = -1;
	monster->queued_target_y = -1;
	monster->xx = monster->tile_x * SPRITE_SIZE;
	monster->yy = monster->tile_y * SPRITE_SIZE;

	return monster;
}

int
py_update_object(adv_object *obj)
{
	obj->tile_x = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "x"));
	obj->tile_y = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "y"));
	obj->type = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "type"));
	obj->animation = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "animation"));

	PyObject_SetAttrString(obj->py_obj, "is_dirty", Py_BuildValue("i", 0));
	return 0;
}

adv_object *
py_new_object_from_pyobj(PyObject *py_obj)
{
	adv_object *obj;

	obj = malloc(sizeof *obj);
	memset(obj, 0, sizeof *obj);
	obj->py_obj = py_obj;
	
	py_update_base_object((adv_base_object*)obj);
	py_update_object(obj);

	return obj;
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


int
py_get_int(PyObject *obj)
{
	int intval;

	if (obj == NULL) {
		PyErr_Print();
		return 0;
	}
	
	if (obj == Py_None)
		return 0;

	intval = PyInt_AsLong(obj);
	return intval;
}

int
py_get_int_decref(PyObject *obj)
{
	int intval;
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
py_update_object_timer(adv_base_object *obj)
{
	if (obj->timer == -1)
		return 0;

	obj->timer --;
	if (obj->timer == 0) {
		PyObject *tmp;
		
		tmp = PyObject_CallMethod(obj->py_obj, "tick", NULL);
		if (tmp == NULL) {
			printf("PyObject_CallMethod(tick):");
			PyErr_Print();
		} else {
			Py_DECREF(tmp);
		}

		/* Update time-counter */
		obj->timer = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "timer"));
		return 1;
	}

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

	if (p == NULL)
		PyErr_Print();

	Py_DECREF(p);
	return 0;
}
