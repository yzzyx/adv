#include <Python.h>
#include "object.h"


void
object_list_free(adv_base_object *list)
{
	adv_base_object *tmp;

	while (list != NULL) {
		tmp = list;
		list = list->next;
		/* FIXME! - what we should do here depends on type! */
		free(tmp);
	}
}

adv_base_object *
object_list_find_pyobj(adv_base_object *list, PyObject *py_obj)
{

	while (list != NULL) {
		if (list->py_obj == py_obj)
			return list;
		list = list->next;
	}
	return NULL;
}

