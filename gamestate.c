#include "gamestate.h"

gamestate global_GS = { NULL, NULL, 0 };

/*
 * gamestate_update()
 *
 * Copy information from C gamestate to python gamestate
 */
int gamestate_update()
{
	if (global_GS.current_map)
	    PyObject_SetAttrString(global_GS.py_obj, "current_map", global_GS.current_map->py_obj);
	return 0;
}

/*
 * gamestate_init()
 *
 * Initialize global gamestate - creates python object, etc.
 */
int gamestate_init()
{
	PyObject *module;
	PyObject *gs_def;
	PyObject *gs;

	module = PyDict_GetItemString(main_dict, "gamestate");
	if (module == NULL) {
		PyErr_Print();
		return -1;
	}
	if ((gs_def = PyObject_GetAttrString(module, "Gamestate")) == NULL) {
		PyErr_Print();
		return -1;
	}

	if ((gs = PyObject_CallObject(gs_def, NULL)) == NULL) {
		PyErr_Print();
		return -1;
	}

	global_GS.py_obj = gs;

	return 0;
}

