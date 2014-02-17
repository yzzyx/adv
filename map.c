#include "animation.h"
#include "map.h"
#include "player.h"
#include "sdl.h"

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

int
render_map(adv_map *m, player *p)
{
	int x;
	int y;
	int start_x, start_y;
	int end_x, end_y;
	int screen_adjust_x = 0, screen_adjust_y = 0;

	int screen_width, screen_height;
	x = 0;
	y = 0;

	screen_width = rs.screen->w / FRAME_WIDTH;
	screen_height = rs.screen->h / FRAME_WIDTH;

	if (m->width < screen_width) {
		start_x = 0; end_x = m->width;
		screen_adjust_x = (screen_width - (end_x - start_x))/2;
	} else {
		start_x = p->tile_x - screen_width / 2;
		if (start_x < 0) start_x = 0;

		end_x = p->tile_x + screen_width / 2;
		if (end_x > m->width) end_x = m->width;

		if ((end_x - start_x) < screen_width) {
			screen_adjust_x = (end_x - start_x) / 2;
		}
	}

	if (m->height < screen_height) {
		start_y = 0; end_y = m->height;
		screen_adjust_y = (screen_height - (end_y - start_y))/2;
	} else {
		start_y = p->tile_y - screen_height / 2;
		if (start_y < 0) start_y = 0;

		end_y = p->tile_y + screen_height / 2;
		if (end_y > m->height) end_y = m->height;

		while (((end_y - start_y) < screen_height) &&
		    (start_y > 0 || end_y < m->height)) {
			if (start_y > 0) start_y --;
			if (end_y < m->height) end_y ++;
		}
		if ((end_y - start_y) < screen_height) {
			screen_adjust_y = (end_y - start_y) / 2;
		}
	}

	for(y = start_y; y < end_y; y ++) {
		for(x = start_x; x < end_x; x ++) {
			render_animation(m->tiles[x+y*m->width]->animation_id,
			    map_to_screen_x(m, x - start_x + screen_adjust_x),
			    map_to_screen_y(m, y - start_y + screen_adjust_y),
			    m->tiles[x+y*m->width]->animation_frame);
		}
	}

	render_animation(p->animation_id,
	    map_to_screen_x(m, p->tile_x - start_x + screen_adjust_x) + p->xx,
	    map_to_screen_y(m, p->tile_y - start_y + screen_adjust_y) + p->yy,
	    p->animation_frame);
	return 0;
}
