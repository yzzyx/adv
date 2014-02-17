#include <Python.h>
#include "common.h"
#include "animation.h"
#include "map.h"

PyObject *main_module;
PyObject *main_dict;

adv_tile EMPTY_TILE = {
	NULL, -1, 0, 0
};


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
    {NULL, NULL, 0, NULL}
};

int
py_get_int(PyObject *obj)
{
	int intval;
	intval = PyInt_AsLong(obj);
	return intval;
}

static int
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
	return t;
}

adv_map *
python_generate_map(const char *map_name)
{
	adv_map *m;
	int width;
	int height;
	PyObject *map_def, *map_inst;
	PyObject *tile_list;
	PyObject *tmp;

	printf("generating map\n");
	PyObject *module = PyDict_GetItemString(main_dict, map_name);
	if (module == NULL) {
		PyErr_Print();
		return NULL;
	}
	if ((map_def = PyObject_GetAttrString(module, map_name)) == NULL) {
		PyErr_Print();
		return NULL;
	}

	if ((map_inst = PyObject_CallObject(map_def, NULL)) == NULL) {
		PyErr_Print();
		return NULL;
	}

	tmp = PyObject_CallMethod(map_inst, "generate", "", NULL);
	if (tmp == NULL) {
		printf("callmethod(generate):");
		PyErr_Print();
		return NULL;
	} else 
		Py_DECREF(tmp);

	m = malloc(sizeof(adv_map));
	m->py_obj = map_inst;
	m->render_start_x = 0;
	m->render_start_y = 0;
	m->width = py_get_int(PyObject_GetAttrString(map_inst, "width"));
	m->height = py_get_int(PyObject_GetAttrString(map_inst, "height"));
	m->tiles = malloc(sizeof(adv_tile *) * m->width *  m->height);

	printf("Map size: %d x %d\n", m->width, m->height);

	PyObject *tile_list_row;
	PyObject *py_tile;
	adv_tile *tile;

	tile_list = PyObject_GetAttrString(map_inst, "tiles");
	if (tile_list == NULL) {
		PyErr_Print();
		return NULL;
	}

	if (PyList_Check(tile_list))
		printf("tile_list is a list!\n");
	else
		printf("tile_list is NOT a list!\n");

	int x, y;
	for (y = 0; y < m->height; y ++) {
		tile_list_row  = PyList_GetItem(tile_list, y);
		if (tile_list_row == NULL) {
			PyErr_Print();
			return NULL;
		}

		for (x = 0; x < m->width; x ++) {
			py_tile = PyList_GetItem(tile_list_row, x);
			if (py_tile == NULL) {
				printf("tile_list[%d][%d]:", y, x);
				PyErr_Print();
				return NULL;
			}
			if (py_tile == Py_None)
				tile = &EMPTY_TILE;
			else
				tile = py_get_tile(py_tile);
			m->tiles[y*m->width + x] = tile;
		}
	}
	return m;
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
