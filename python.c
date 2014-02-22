#include <Python.h>
#include "common.h"
#include "animation.h"
#include "map.h"

PyObject *main_module;
PyObject *main_dict;


PyObject *loadAnimation(PyObject *self, PyObject *args)
{
	const char *filename;
	int animation_id = 0;

	if (!PyArg_ParseTuple(args, "s", &filename)) {
		printf("loadAnimation():\n");
		PyErr_Print();
		return NULL;
	}

	animation_id = load_animation(filename);

	PyObject *anim = PyDict_New();
	PyDict_SetItemString(anim, "frames",
	    Py_BuildValue("i", animation_get_n_frames(animation_id)));
	PyDict_SetItemString(anim, "current_frame", Py_BuildValue("i", 0));
	PyDict_SetItemString(anim, "id", Py_BuildValue("i", animation_id));

	printf("loadAnimation(%s): %d!\n",filename, animation_id);
	return anim;
}

static PyMethodDef methods[] = {
    {"loadAnimation", loadAnimation, METH_VARARGS, "loadAnimation" },
//    {"gotoPosition", monster_gotoPosition, METH_VARARGS, "gotoPosition" },
    {NULL, NULL, 0, NULL}
};


int
py_get_int(PyObject *obj)
{
	int intval;
	intval = PyInt_AsLong(obj);
	return intval;
}

int
py_get_int_decref(PyObject *obj)
{
	int intval;
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
	t->walkable = py_get_int_decref(PyObject_GetAttrString(py_obj, "walkable"));
	t->visibility = py_get_int_decref(PyObject_GetAttrString(py_obj, "visibility"));
	return t;
}

int setup_python(int argc, char *argv[])
{
	PyObject *adv_module;
	Py_SetProgramName(argv[0]);
	Py_Initialize();
	
	Py_InitModule("adv", methods);
	adv_module = PyImport_ImportModule("adv");
	PyImport_AddModule("adv");
	if (adv_module == NULL) { PyErr_Print(); return -1; }

	main_module = PyImport_AddModule("__main__");
	main_dict = PyModule_GetDict(main_module);
	if (main_module == NULL || main_dict == NULL) { PyErr_Print(); return -1; }

	FILE *fp = fopen("load.py", "r");
	PyObject *p = PyRun_File(fp, "load.py", Py_file_input, main_dict, main_dict);
	fclose(fp);

	if (p == NULL)
		PyErr_Print();

	Py_DECREF(p);
	return 0;
}
