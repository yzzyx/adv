#include <Python.h>
#include "common.h"
#include "animation.h"
#include "player.h"
#include "map.h"
#include "python.h"

PyObject *main_module;
PyObject *main_dict;


PyObject *py_animLoadSpritesheet(PyObject *self, PyObject *args)
{
	const char *filename;
	int ss_id = 0;

	if (!PyArg_ParseTuple(args, "s", &filename)) {
		printf("py_loadSpritesheet():\n");
		PyErr_Print();
		return NULL;
	}

	ss_id = animation_load_spritesheet(filename);

	PyObject *py_ss = Py_BuildValue("i", ss_id);
	/*PyDict_New();
	PyDict_SetItemString(py_ss, "frames",
	    Py_BuildValue("i", animation_spritesheet_get_n_sprites(ss_id)));
	PyDict_SetItemString(py_ss, "id", Py_BuildValue("i", ss_id));
	*/

	printf("loadSpritesheet(%s): %d!\n",filename, ss_id);
	return py_ss;
}

PyObject *py_animCreate(PyObject *self, PyObject *args)
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

PyObject *py_getPlayer(PyObject *self, PyObject *args)
{

	Py_INCREF(main_player->py_obj);
	return main_player->py_obj;
}

int
pyobj_is_dirty(PyObject *obj)
{
	return py_get_int_decref(PyObject_GetAttrString(obj, "is_dirty"));
}

int 
py_update_base_object(adv_base_object *obj)
{
	obj->timer = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "timer"));
	obj->has_directions = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "has_directions"));

	if (obj->has_directions)
		obj->direction = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "direction"));
	else
		obj->direction = DIRECTION_UP;
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

	if (monster->xx % FRAME_WIDTH != monster->tile_x)
		monster->xx = monster->tile_x * FRAME_WIDTH;
	if (monster->yy % FRAME_WIDTH != monster->tile_x)
		monster->yy = monster->tile_y * FRAME_WIDTH;

	monster->target_tile_x = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "target_x"));
	monster->target_tile_y = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "target_y"));
	monster->speed = py_get_int_decref(PyObject_GetAttrString(monster->py_obj, "speed"));
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
py_new_monster_from_object(PyObject *obj)
{
	adv_monster *monster;

	monster = malloc(sizeof *monster);
	memset(monster, 0, sizeof *monster);
	monster->py_obj = obj;
	py_update_base_object((adv_base_object*)monster);
	py_update_monster(monster);

	return monster;
}

static PyMethodDef methods[] = {
    {"loadSpritesheet", py_animLoadSpritesheet, METH_VARARGS, "load spritesheet from file" },
    {"createAnimation", py_animCreate, METH_VARARGS, "create animation from spritesheet" },
    {"getPlayer", py_getPlayer, METH_VARARGS, "get the player object" },
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
		Py_DECREF(tmp);

		/* Update time-counter */
		obj->timer = py_get_int_decref(PyObject_GetAttrString(obj->py_obj, "timer"));
		return 1;
	}

	return 0;
}


int setup_python(int argc, char *argv[])
{
	PyObject *adv_module;

	Py_SetProgramName(argv[0]);
	Py_SetPythonHome(".\\python");

	Py_Initialize();
	
	Py_InitModule("adv", methods);
	adv_module = PyImport_ImportModule("adv");
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
