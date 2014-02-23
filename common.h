#ifndef COMMON_H
#define COMMON_H
#include <SDL.h>
#include <SDL_image.h>
#include <Python.h>

#define TILE_WIDTH 32
#define TILE_HEIGHT 32

#define SCREEN_HEIGHT 640
#define SCREEN_WIDTH 640
#define TICK_TIMEOUT 10

#define RESOURCE_DIR "res/"

#define FRAMES_PER_SECOND 60


struct t_adv_base_object;
#define ADV_OBJECT_HEADER \
	PyObject *py_obj; \
	int timer;  \
	struct t_adv_base_object *next; \
	struct t_adv_base_object *prev;

typedef struct t_adv_base_object {
	ADV_OBJECT_HEADER
}adv_base_object;

#define ADV_OBJECT_LIST \

typedef struct t_render_space {
	SDL_Window *win;
	SDL_Surface *screen;
	SDL_Surface *map_surface;
	int fog_tile;
}render_space;

#endif /* end of include guard: COMMON_H */
