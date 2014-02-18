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
	static int prev_start_x = -1,
		   prev_start_y = -1,
		   prev_end_x = -1,
		   prev_end_y = -1,
		   prev_clip_x = -1,
		   prev_clip_y = -1;
	int x, y;
	int start_x, start_y;
	int end_x, end_y;
	int screen_width, screen_height;
	int blit;

	screen_width = rs.screen->w / FRAME_WIDTH;
	screen_height = rs.screen->h / FRAME_WIDTH;

	SDL_Rect clip;
	SDL_Rect screen_rect;

	clip.x = 0;
	clip.y = 0;
	clip.w = rs.screen->w;
	clip.h = rs.screen->h;
	memcpy(&screen_rect, &clip, sizeof(SDL_Rect));
	int map_scroll_y = 0, map_scroll_x;

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

		if(p->xx != 0 && p->tile_x == (start_x + screen_width / 2)) {
			if (p->xx < 0 && start_x > 0) {
				/* We have to scroll the map to the left */
				start_x --;
				end_x --;
				map_scroll_x = p->xx;
				clip.x = FRAME_WIDTH + map_scroll_x;
			} else if (p->xx > 0 && end_x - p->tile_x  > screen_width / 2) {
				/* We have to scroll the map to the right */
				map_scroll_x = p->xx;
				clip.x += map_scroll_x;
			}
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

		if(p->yy != 0 && p->tile_y == (start_y + screen_height / 2)) {
			if (p->yy < 0 && start_y > 0) {
				/* We have to scroll the map up */
				start_y --;
				end_y --;
				map_scroll_y = p->yy;
				clip.y = FRAME_HEIGHT + map_scroll_y;
			} else if (p->yy > 0 && end_y - p->tile_y  > screen_height / 2) {
				/* We have to scroll the map down */
				map_scroll_y = p->yy;
				clip.y += map_scroll_y;
			}
		}
	}


	/* Only blit if something changed */
	if (start_y != prev_start_y || 
	    start_x != prev_start_x ||
	    end_y != prev_end_y ||
	    end_x != prev_end_x ||
	    clip.x != prev_clip_x ||
	    clip.y != prev_clip_x) {

		prev_start_x = start_x;
		prev_start_y = start_y;
		prev_end_x = end_x;
		prev_end_y = end_y;
		prev_clip_x = clip.x;
		prev_clip_y = clip.y;

		blit = 1;

		/* Clear whole map, we need to move it */
		SDL_FillRect(rs.map_surface, NULL, 0);
	}

	/* Even if we're not blitting anything, we need to check if any of the
	 * tiles are dirty
	 */
	for(y = start_y; y < end_y; y ++) {
		for(x = start_x; x < end_x; x ++) {
			if (blit) // FIXME - check if dirty || m->tiles[x+y*m->width]->is_dirty) {
				render_animation_full(m->tiles[x+y*m->width]->animation_id,
				    m->tiles[x+y*m->width]->animation_frame,
				    map_to_screen_x(m, x - start_x),
				    map_to_screen_y(m, y - start_y),
				    rs.map_surface);
		}
	}

	//printf("from %d,%d to %d,%d\n", start_x, start_y, end_x, end_y);
	SDL_SetAlpha(rs.map_surface, 0, 0xFF);
	SDL_BlitSurface(rs.map_surface, &clip, rs.screen, &screen_rect);

	render_animation(p->animation_id,
	    map_to_screen_x(m, p->tile_x - start_x) + screen_rect.x + p->xx - clip.x,
	    map_to_screen_y(m, p->tile_y - start_y) + screen_rect.y + p->yy - clip.y,
	    p->animation_frame);
	return 0;
}
