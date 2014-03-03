#include <Python.h>
#include "python.h"
#include "animation.h"
#include "map.h"
#include "player.h"
#include "sdl.h"

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

	PyObject *monster_list;
	PyObject *py_monster;
	adv_monster *monster;
	monster_list = PyObject_GetAttrString(map_inst, "monsters");
	if (monster_list == NULL) {
		PyErr_Print();
		free(m);
		return NULL;
	}

	for (x = 0; x < PyList_Size(monster_list); x ++) {
		py_monster = PyList_GetItem(monster_list, x);
		if (py_monster == Py_None)
			continue;
		else
			monster = py_new_monster_from_object(py_monster);

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

	return m;
}


static int 
map_to_screen_x(adv_map *m, int x)
{
	int sx;

	sx = x - m->render_start_x;
	sx *= FRAME_WIDTH;
	return sx;
}

static int
map_to_screen_y(adv_map *m, int y)
{
	int sy;

	sy = y - m->render_start_y;
	sy *= FRAME_HEIGHT;
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

	x = map_x * FRAME_WIDTH;
	y = map_y * FRAME_HEIGHT;

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
		tx = x / FRAME_WIDTH;
		ty = y / FRAME_HEIGHT;
		if (m->tiles[tx+ty*m->width]->visibility == 0) {
			if (!(tx == map_x / FRAME_WIDTH && 
				ty == map_y / FRAME_HEIGHT))
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
	static int prev_x = -1,
		   prev_y = -1;
	int x, y;
	int start_x, start_y;
	int end_x, end_y;
	int screen_width, screen_height;

	screen_width = rs.screen->w / FRAME_WIDTH;
	screen_height = rs.screen->h / FRAME_WIDTH;

	SDL_Rect clip;
	SDL_Rect screen_rect;

	if (m->map_surface == NULL) {
		m->map_surface = surface_sdl(m->width * FRAME_WIDTH,
		    m->height * FRAME_HEIGHT);
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
		m->fog_surface = surface_sdl(m->width * FRAME_WIDTH,
		    m->height * FRAME_HEIGHT);
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

	clip.x = 0;
	clip.y = 0;
	clip.w = rs.screen->w;
	clip.h = rs.screen->h;
	memcpy(&screen_rect, &clip, sizeof(SDL_Rect));
	int map_scroll_y = 0, map_scroll_x = 0;

	start_x = p->tile_x - screen_width / 2;
	start_y = p->tile_y - screen_height / 2;
	end_x = p->tile_x + screen_width / 2;
	end_y = p->tile_y + screen_height / 2;

	if (m->width < screen_width) {
		start_x = 0; end_x = m->width;
		clip.w = m->width * FRAME_WIDTH;
		screen_rect.w = clip.w;
		screen_rect.x = (rs.screen->w -
		    ((end_x - start_x)*FRAME_WIDTH))/2;
	} else if (m->width > screen_width) {
		if (start_x < 0) start_x = 0;

		end_x = start_x + screen_width + 1;
		if (end_x > m->width) {
			end_x = m->width;
			start_x = m->width - screen_width;
		}

		if(p->xx % FRAME_WIDTH > 0 &&
		    p->tile_x == (start_x + screen_width / 2) &&
		    m->width - p->tile_x > screen_width/2) {
			map_scroll_x = p->xx - (start_x + screen_width/2) * FRAME_WIDTH;
		}
	}

	if (m->height < screen_height) {
		start_y = 0; end_y = m->height;
		clip.h = m->height * FRAME_WIDTH;
		screen_rect.h = clip.h;
		screen_rect.y = (rs.screen->h -
		    ((end_y - start_y)*FRAME_HEIGHT))/2;
	} else if (m->height > screen_height) {
		if (start_y < 0) start_y = 0;

		end_y = start_y + screen_height + 1;
		if (end_y > m->height) {
			end_y = m->height;
			start_y = m->height - screen_height;
		}

		if(p->yy % FRAME_HEIGHT > 0 &&
		    p->tile_y == (start_y + screen_height / 2) &&
		    m->height - p->tile_y > screen_height/2 + 1) {
			map_scroll_y = p->yy - (start_y + screen_height/2) * FRAME_WIDTH;
		}
	}


	/* Only blit if something changed */
	if (p->xx != prev_x || 
	    p->yy != prev_y) {

		prev_x = p->xx;
		prev_y = p->yy;

#if USE_FOG

		/* Create a visibility-map */
		SDL_FillRect(m->fog_surface, NULL, 0);

		memset(m->fog_map, 0, m->width * m->height);
		for (x = start_x * FRAME_WIDTH; x < end_x * FRAME_WIDTH; x += 16) {
			for (y = start_y * FRAME_HEIGHT; y < end_y *
			    FRAME_HEIGHT; y += 16) {

				if (m->fog_map[(x>>5) + (y>>5) * m->width] == 0 &&
				   !map_pos_is_visible2(m, p, x, y)) {
					render_animation_clipped(rs.fog_tile,
					    0, x, y, 16, 16,
					    m->fog_surface);
				} else {
					m->fog_map[(x>>5) + (y>>5) * m->width]= 1;
					SDL_Rect r;
					r.x = (x >> 5) * FRAME_WIDTH;
					r.y = (y >> 5) * FRAME_HEIGHT;
					r.w = 32; r.h = 32;
					SDL_FillRect(m->fog_surface, &r, 0);
				}
			}
		}
#endif
	}

	/* Even if we're not blitting anything, we need to check if any of the
	 * tiles are dirty
	 */
	clip.x = start_x * FRAME_WIDTH + map_scroll_x;
	clip.y = start_y * FRAME_HEIGHT + map_scroll_y;
	clip.w = screen_width * FRAME_WIDTH;
	clip.h = screen_height * FRAME_WIDTH;
	SDL_BlitSurface(m->map_surface, &clip, rs.screen, &screen_rect);
#if USE_FOG
	SDL_BlitSurface(m->fog_surface, &clip, rs.screen, &screen_rect);
#endif
	

	if (p->in_movement) {
		animation_render(p->animation_moving[p->direction],
		    p->xx - start_x * FRAME_WIDTH - map_scroll_x + screen_rect.x,
		    p->yy - start_y * FRAME_WIDTH - map_scroll_y + screen_rect.y);
	} else {
		animation_render(p->animation_stopped[p->direction],
		    p->xx - start_x * FRAME_WIDTH - map_scroll_x + screen_rect.x,
		    p->yy - start_y * FRAME_WIDTH - map_scroll_y + screen_rect.y);
	}

	adv_monster *monster;

	monster = m->monsters;
	for (; monster != NULL; monster = (adv_monster *)monster->next) {
		if (monster->tile_x >= start_x && monster->tile_x <= end_x &&
		    monster->tile_y >= start_y && monster->tile_y <= end_y)
			animation_render(monster->animation_stopped[monster->direction],
			    monster->xx - start_x * FRAME_WIDTH - map_scroll_x + screen_rect.x,
			    monster->yy - start_y * FRAME_WIDTH - map_scroll_y + screen_rect.y);
	}
	/* Return 1 if we've actually done something */
	return 1;
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
		move_player(monster);
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
map_is_walkable(adv_monster *m, adv_map *map, int x, int y)
{

	if (map->tiles[x+y*map->width]->walkable == 0)
		return 0;

	/* FIXME - don't use global player */
	if (m != main_player &&
	    main_player->tile_x == x && main_player->tile_y == y)
		return 0;

	adv_monster *monster;

	monster = map->monsters;
	for (; monster != NULL; monster = (adv_monster *)monster->next) {
		if (m != monster && monster->tile_x == x && monster->tile_y == y)
			return 0;
	}
	return 1;
}
