#ifndef MAP_H
#define MAP_H
#include <Python.h>
#include <SDL.h>
#include "common.h"
#include "player.h"
#include "object.h"
#include "astar.h"

typedef struct t_adv_tile {
	ADV_OBJECT_HEADER

	int spritesheet;
	int spriteid;

	int walkable;
	int visibility;
}adv_tile;

typedef struct t_adv_map {
	ADV_OBJECT_HEADER

	int width;
	int height;

	int tile_width;
	int tile_height;

	SDL_Surface *map_surface;
	SDL_Surface *fog_surface;
	adv_tile **tiles;
	PyObject *monster_list;
	PyObject *object_list;

	pathfinder_info *pathfinder;
	uint8_t *fog_map;
	uint8_t *raytrace_map;
	int render_start_x;
	int render_start_y;
}adv_map;

adv_map *get_map(const char *map_name);
int render_map(adv_map *m);
int map_get_position_from_screen(int screen_x, int screen_y, int *tile_x, int *tile_y);

int update_map_monsters(adv_map *m);
int call_tick_map(adv_map *m);
int call_tick_map_monsters(adv_map *m);
int call_tick_map_objects(adv_map *m);

int map_tile_is_walkable(int x, int y, void *monster);
int map_is_walkable(PyObject *m, int x, int y);
int map_update_monster_animations(adv_map *map);
int map_has_line_of_sight(int x1, int y1, int x2, int y2);

#endif /* end of include guard: MAP_H */
