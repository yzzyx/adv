#include "animation.h"
#include "map.h"
#include "player.h"

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

	x = 0;
	y = 0;

	for(y = 0; y < m->height; y ++) {
		for(x = 0; x < m->width; x ++) {
			render_animation(m->tiles[x+y*m->width]->animation_id,
			    map_to_screen_x(m, x), map_to_screen_y(m, y),
			    m->tiles[x+y*m->width]->animation_frame);
		}
	}

	render_animation(p->animation_id,
	    map_to_screen_x(m, p->tile_x) + p->xx,
	    map_to_screen_y(m, p->tile_y) + p->yy,
	    p->animation_frame);
	return 0;
}
