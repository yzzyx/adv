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
		printf("gamestate_init:GetItemString():");
		PyErr_Print();
		return -1;
	}
	if ((gs_def = PyObject_GetAttrString(module, "Gamestate")) == NULL) {
		printf("gamestate_init:GetAttrString():");
		PyErr_Print();
		return -1;
	}

	if ((gs = PyObject_CallObject(gs_def, NULL)) == NULL) {
		printf("gamestate_init:CallObject():");
		PyErr_Print();
		return -1;
	}

	global_GS.py_obj = gs;

	return 0;
}

