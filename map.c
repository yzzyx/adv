#include <Python.h>
#include "python.h"
#include "animation.h"
#include "map.h"
#include "player.h"
#include "sdl.h"
#include "gamestate.h"

#define USE_FOG 0
int map_count = 0;

adv_tile EMPTY_TILE = {
	NULL, -1, 0, 0, 0,
};


adv_map *
get_map(const char *map_name)
{
	adv_map *m;

	PyObject *map_def, *map_inst;
	PyObject *tile_list;
	PyObject *tmp;

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
	memset(m, 0, sizeof(adv_map));

	m->py_obj = map_inst;

	py_update_base_object((adv_base_object*)m);

	m->render_start_x = 0;
	m->render_start_y = 0;
	m->width = py_get_int(PyObject_GetAttrString(map_inst, "width"));
	m->height = py_get_int(PyObject_GetAttrString(map_inst, "height"));
	m->tiles = malloc(sizeof(adv_tile *) * m->width *  m->height);

	printf("Map size: %d x %d\n", m->width, m->height);
	printf("tiles = %p\n", m->tiles);

	PyObject *tile_list_row;
	PyObject *py_tile;
	adv_tile *tile;

	tile_list = PyObject_GetAttrString(map_inst, "tiles");
	if (tile_list == NULL) {
		PyErr_Print();
		free(m);
		return NULL;
	}

	int x, y;
	for (y = 0; y < m->height; y ++) {
		tile_list_row  = PyList_GetItem(tile_list, y);
		if (tile_list_row == NULL) {
			PyErr_Print();
			free(m);
			return NULL;
		}

		for (x = 0; x < m->width; x ++) {
			py_tile = PyList_GetItem(tile_list_row, x);
			if (py_tile == NULL) {
				printf("tile_list[%d][%d]:", y, x);
				PyErr_Print();
				free(m);
				return NULL;
			}
			if (py_tile == Py_None)
				tile = &EMPTY_TILE;
			else
				tile = py_get_tile(py_tile);
			m->tiles[y*m->width + x] = tile;
		}
	}

	PyObject *list;
	PyObject *py_obj;
	adv_monster *monster;
	adv_object *object;

	/* Get all monsters currently on the map */
	list = PyObject_GetAttrString(map_inst, "monsters");
	if (list == NULL) {
		PyErr_Print();
		free(m);
		return NULL;
	}

	for (x = 0; x < PyList_Size(list); x ++) {
		py_obj = PyList_GetItem(list, x);
		if (py_obj == Py_None)
			continue;
		else
			monster = py_new_monster_from_pyobj(py_obj);

		monster->map = m;
		if (m->monsters) {
			m->monsters->prev = (adv_base_object*)monster;
			monster->next = (adv_base_object*)m->monsters;
			monster->prev = NULL;
		} else {
			monster->next = NULL;
			monster->prev = NULL;
		}
		m->monsters = monster;
	}

	/* Get all objects currently on the map */
	list = PyObject_GetAttrString(map_inst, "objects");
	if (list == NULL) {
		PyErr_Print();
		free(m);
		return NULL;
	}

	for (x = 0; x < PyList_Size(list); x ++) {
		py_obj = PyList_GetItem(list, x);
		if (py_obj == Py_None)
			continue;
		else
			object = py_new_object_from_pyobj(py_obj);

		if (m->objects) {
			m->objects->prev = (adv_base_object*)object;
			object->next = (adv_base_object*)m->objects;
			object->prev = NULL;
		} else {
			object->next = NULL;
			object->prev = NULL;
		}
		m->objects = object;
	}
	printf("%d objects!\n", x);
	return m;
}


static int 
map_to_screen_x(adv_map *m, int x)
{
	int sx;

	sx = x - m->render_start_x;
	sx *= SPRITE_SIZE;
	return sx;
}

static int
map_to_screen_y(adv_map *m, int y)
{
	int sy;

	sy = y - m->render_start_y;
	sy *= SPRITE_SIZE;
	return sy;
}

#if USE_FOG
static int
map_pos_is_visible(adv_map *m, player *p, int map_x, int map_y)
{
	int dx = 0, dy = 0;
	int x,y;
	int sx, sy;
	int err;
	
	dx = abs(p->tile_x-map_x);
	dy = abs(p->tile_y-map_y);
	if (map_x < p->tile_x) sx = 1; else sx = -1;
	if (map_y < p->tile_y) sy = 1; else sy = -1;
	err = dx-dy;
	x = map_x;
	y = map_y;

	for(;;) {
		if (m->tiles[x+y*m->width]->visibility == 0) {
			if (!(x == map_x && y == map_y))
				return 0;
		}

		if (x == p->tile_x && y == p->tile_y)
			break;
		int e2 = 2 * err;
		if (e2 > -dy) {
			err = err - dy;
			x += sx;
		}
		if (x == p->tile_x && y == p->tile_y) {
//			if (m->tiles[x+y*m->width]->visibility == 0)
//				return 0;
			break;
		}
		if (e2 < dx) {
			err = err + dx;
			y += sy;
		}
	}



	/*j
	if (p->tile_x > map_x) dx = 1;
	else if (p->tile_x < map_x) dx = -1;

	if (p->tile_y > map_y) dy = 1;
	else if (p->tile_y < map_y) dy = -1;

	x = map_x * SPRITE_SIZE;
	y = map_y * SPRITE_SIZE;

	while (x != p->tile_x  || y != p->tile_y) {
		if (x != map_x && y != map_y &&
		    m->tiles[x+y*m->width]->visibility == 0)
			return 0;
		if (x != p->tile_x) x += dx;
		if (y != p->tile_y) y += dy;
	}
	*/
	return 1;
}

static int
map_pos_is_visible2(adv_map *m, player *p, int map_x, int map_y)
{
	int dx = 0, dy = 0;
	int x,y;
	int sx, sy;
	int err;
	int px, py;
	

	px = p->xx + 16;
	py = p->yy + 16;
	dx = abs(px-map_x);
	dy = abs(py-map_y);
	if (map_x < px) sx = 1; else sx = -1;
	if (map_y < py) sy = 1; else sy = -1;
	err = dx-dy;
	x = map_x;
	y = map_y;

	for(;;) {
		int tx, ty;
		tx = x / SPRITE_SIZE;
		ty = y / SPRITE_SIZE;
		if (m->tiles[tx+ty*m->width]->visibility == 0) {
			if (!(tx == map_x / SPRITE_SIZE && 
				ty == map_y / SPRITE_SIZE))
				return 0;
		}

		if (x == px && y == py)
			break;
		int e2 = 2 * err;
		if (e2 > -dy) {
			err = err - dy;
			x += sx;
		}
		if (x == px && y == py) {
//			if (m->tiles[x+y*m->width]->visibility == 0)
//				return 0;
			break;
		}
		if (e2 < dx) {
			err = err + dx;
			y += sy;
		}
	}
	return 1;
}
#endif


int
render_map(adv_map *m, player *p)
{
	int x, y;

	if (m->map_surface == NULL) {
		m->map_surface = surface_sdl(m->width * SPRITE_SIZE,
		    m->height * SPRITE_SIZE);
		SDL_SetSurfaceBlendMode(m->map_surface, SDL_BLENDMODE_NONE);

		for(x = 0; x < m->width; x ++) {
			for(y = 0; y < m->height; y ++) {
				animation_render_sprite_full(m->tiles[x+y*m->width]->spritesheet,
				    m->tiles[x+y*m->width]->spriteid,
				    map_to_screen_x(m, x),
				    map_to_screen_y(m, y),
				    m->map_surface);

			}
		}
	}

#if USE_FOG
	if (m->fog_surface == NULL) {
		m->fog_surface = surface_sdl(m->width * SPRITE_SIZE,
		    m->height * SPRITE_SIZE);
//		SDL_SetColorKey(m->fog_surface, SDL_TRUE, 0);
		printf("[fog] format: %d\n",  m->fog_surface->format->format);
		printf("[fog] BPP: %d\n",  m->fog_surface->format->BitsPerPixel);
		printf("[fog] ByPP: %d\n", m->fog_surface->format->BytesPerPixel);
		printf("[fog] RMask: %.8x\n",  m->fog_surface->format->Rmask);
		printf("[fog] GMask: %.8x\n",  m->fog_surface->format->Gmask);
		printf("[fog] BMask: %.8x\n",  m->fog_surface->format->Bmask);
		printf("[fog] AMask: %.8x\n",  m->fog_surface->format->Amask);

		SDL_SetSurfaceBlendMode(m->fog_surface, SDL_BLENDMODE_BLEND);

		m->fog_map = malloc(m->width * m->height);
	}
#endif
	int start_x, start_y;
	int end_x, end_y;
	int screen_width, screen_height;
	int map_width, map_height;

	SDL_Rect clip;

	map_width = m->width * SPRITE_SIZE;
	map_height = m->height * SPRITE_SIZE;
	screen_width = rs.screen->w;
	screen_height = rs.screen->h;

	if (p->xx < screen_width / 2) {
		start_x = 0;
		end_x = screen_width;
	} else if (p->xx > map_width - screen_width / 2) {
		end_x = map_width;
		start_x = map_width - screen_width;
	} else {
		start_x = p->xx - screen_width / 2;
		end_x = p->xx + screen_width / 2;
	}

	if (p->yy < screen_height / 2) {
		start_y = 0;
		end_y = screen_width;
	} else if (p->yy > map_height - screen_height / 2) {
		end_y = map_height;
		start_y = map_height - screen_height;
	} else {
		start_y = p->yy - screen_height / 2;
		end_y = p->yy + screen_height / 2;
	}


	if (start_x < 0) start_x = 0;
	if (start_y < 0) start_y = 0;
	if (end_x > screen_width) end_x = screen_width;
	if (end_y > screen_height) end_y = screen_height;

	clip.x = start_x;
	clip.y = start_y;
	clip.w = end_x;
	clip.h = end_y;
	SDL_BlitSurface(m->map_surface, &clip, rs.screen, NULL);
#if USE_FOG
	SDL_BlitSurface(m->fog_surface, &clip, rs.screen, &screen_rect);
#endif
	/* Draw objects */
	adv_object *obj;

	obj = m->objects;
	for (; obj != NULL; obj = (adv_object *)obj->next) {
		x = obj->tile_x * SPRITE_SIZE;
		y = obj->tile_y * SPRITE_SIZE;

		if (x - start_x >= 0 &&
			x - start_x < screen_width &&
			y - start_y >= 0 &&
			y - start_y < screen_height) {
				animation_render(obj->animation,
					x - start_x, y - start_y);
		}
	}

	/* Draw player */
	int dir = 0;
	if (p->has_directions) dir = p->direction;
	if (p->draw_movement) {
		animation_render(p->animation_moving[dir],
		    p->xx - start_x,
		    p->yy - start_y);
	} else {
		animation_render(p->animation_stopped[dir],
		    p->xx - start_x,
		    p->yy - start_y);
	}

	/* Draw monsters */
	adv_monster *monster;

	monster = m->monsters;
	for (; monster != NULL; monster = (adv_monster *)monster->next) {
		if (monster->has_directions) dir = monster->direction;
		else dir = 0;

		if (monster->xx - start_x >= 0 &&
		    monster->xx - start_x < screen_width &&
		    monster->yy - start_y >= 0 &&
		    monster->yy - start_y < screen_height) {
			if (monster->draw_movement) {
				animation_render(monster->animation_moving[dir],
				    monster->xx - start_x,
				    monster->yy - start_y);
			} else {
				animation_render(monster->animation_stopped[dir],
				    monster->xx - start_x,
				    monster->yy - start_y);
			}
		}
	}

	/* Draw attack-direction */

	float angle = atan2f(p->attack_target_x - p->tile_x,
						 p->attack_target_y - p->tile_y) * 180/M_PI;

	dir = -1;
	/* Straight directions */
	if (angle >   0 - 45.0/2 && angle <=   0 + 45.0/2) dir = DIRECTION_DOWN;
	if (angle >  90 - 45.0/2 && angle <=  90 + 45.0/2) dir = DIRECTION_RIGHT;
	if (angle > 180 - 45.0/2 && angle <= 180 + 45.0/2) dir = DIRECTION_UP;
	if (angle > -90 - 45.0/2 && angle <= -90 + 45.0/2) dir = DIRECTION_LEFT;

	/* Corner directions */
	if (angle >   45 - 45.0/2 && angle <=   45 + 45.0/2) dir = 4; /* DOWN+RIGHT */
	if (angle >  -45 - 45.0/2 && angle <=  -45 + 45.0/2) dir = 5; /* DOWN+LEFT */
	if (angle >  135 - 45.0/2 && angle <=  135 + 45.0/2) dir = 7; /* UP+RIGHT */
	if (angle > -135 - 45.0/2 && angle <= -135 + 45.0/2) dir = 6; /* UP+LEFT */

	if (dir > -1)
	animation_render_sprite(rs.attack_cursor_sprites, dir, p->xx - start_x, p->yy - start_y);

	adv_animation_list *l;
	l = rs.animation_list;
	while (l) {
		x = l->x * SPRITE_SIZE - start_x;
		y = l->y * SPRITE_SIZE - start_y;
		if (x >= 0 &&
			x < screen_width &&
			y >= 0 &&
			y < screen_height) {
			animation_render(l->id, x, y);
		}
		l = l->next;
	}

	/* Return 1 if we've actually done something */
	return 1;
}

int
map_get_tile_position_from_screen(int screen_x, int screen_y, int *tile_x, int *tile_y)
{
	int start_x, start_y;
	int screen_width, screen_height;
	int map_width, map_height;

	adv_map *m = global_GS.current_map;
	adv_monster *p = main_player;

	map_width = m->width * SPRITE_SIZE;
	map_height = m->height * SPRITE_SIZE;
	screen_width = rs.screen->w;
	screen_height = rs.screen->h;

	if (p->xx < screen_width / 2) {
		start_x = 0;
	} else if (p->xx > map_width - screen_width / 2) {
		start_x = map_width - screen_width;
	} else {
		start_x = p->xx - screen_width / 2;
	}

	if (p->yy < screen_height / 2) {
		start_y = 0;
	} else if (p->yy > map_height - screen_height / 2) {
		start_y = map_height - screen_height;
	} else {
		start_y = p->yy - screen_height / 2;
	}

	if (start_x < 0) start_x = 0;
	if (start_y < 0) start_y = 0;

   *tile_x = 
	   p->tile_x +
	   (screen_x*
	   ((float)rs.screen->w / rs.real_screen->w) - (p->xx - start_x)) /
		SPRITE_SIZE;
	*tile_y = 
		p->tile_y +
		(screen_y*
		((float)rs.screen->h / rs.real_screen->h) - (p->yy - start_y)) /
		SPRITE_SIZE;
	return 0;
}

int
call_tick_map(adv_map *m)
{
	return py_update_object_timer((adv_base_object*)m);
}

int call_tick_map_monsters(adv_map *m)
{
	adv_monster *monster;

	monster = m->monsters;
	for (; monster != NULL; monster = (adv_monster *)monster->next) {
		py_update_object_timer((adv_base_object*)monster);
	}
	return 0;
}

int update_map_monsters(adv_map *m)
{
	adv_monster *monster;

	monster = m->monsters;
	for (; monster != NULL; monster = (adv_monster *)monster->next) {
		if (monster->is_dirty) {
			py_update_monster_from_c(monster);
		} else {
			if (pyobj_is_dirty(monster->py_obj)) {
				py_update_monster(monster);
			}
		}
		monster_move(monster);
	}
	return 0;
}


int call_tick_map_objects(adv_map *m)
{
	adv_object *obj;

	obj = m->objects;
	for (; obj != NULL; obj = (adv_object *)obj->next) {
		py_update_object_timer((adv_base_object*)obj);
	}
	return 0;
}

int
map_tile_is_walkable(adv_map *m, int x, int y)
{

	if (m->tiles[x+y*m->width]->walkable == 0)
		return 0;
	return 1;
}

int
map_is_walkable(adv_monster *m, int x, int y)
{
	adv_map *map;

	map = global_GS.current_map;

	if (x < 0 || y < 0) return 0;
	if (x >= map->width || y >= map->height) return 0;

	if (map->tiles[x+y*map->width]->walkable == 0)
		return 0;

	/* Allow the user to go to it's own location */
	if (x == m->tile_x && y == m->tile_y)
		return 1;

	/* FIXME - don't use global player */
	if (m != main_player &&
	    main_player->tile_x == x && main_player->tile_y == y)
		return 0;

	adv_monster *monster;

	monster = map->monsters;
	for (; monster != NULL; monster = (adv_monster *)monster->next) {
		if (m == monster)
			continue;
		if (monster->tile_x == x && monster->tile_y == y)
			return 0;

		if (monster->in_movement) {
			int mx = 0,my = 0;
			switch(monster->direction) {
				case DIRECTION_UP: mx = 0; my = -1; break;
				case DIRECTION_DOWN: mx = 0; my = 1; break;
				case DIRECTION_LEFT: mx = -1; my = 0; break;
				case DIRECTION_RIGHT: mx = 1; my = 0; break;
				default: break;
			}

			if (monster->tile_x + mx == x && monster->tile_y + my == y)
				return 0;
		}
	}
	return 1;
}

int
map_update_monster_animations(adv_map *map)
{
	adv_monster *monster;
	int dir;

	monster = map->monsters;
	for (; monster != NULL; monster = (adv_monster *)monster->next) {
		if (monster->has_directions) dir = monster->direction;
		else dir = 0;

		if (monster->draw_movement)
			animation_next_clip(monster->animation_moving[dir]);
		else
			animation_next_clip(monster->animation_stopped[dir]);

		if (!monster->in_movement)
			monster->draw_movement = 0;
	}
	return 1;

}
