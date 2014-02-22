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

	if (m->map_surface == NULL) {
		m->map_surface = surface_sdl(m->width * FRAME_WIDTH,
		    m->height * FRAME_HEIGHT);
		SDL_SetSurfaceBlendMode(m->map_surface, SDL_BLENDMODE_NONE);

		for(x = 0; x < m->width; x ++) {
			for(y = 0; y < m->height; y ++) {
				render_animation_full(m->tiles[x+y*m->width]->animation_id,
				    m->tiles[x+y*m->width]->animation_frame,
				    map_to_screen_x(m, x),
				    map_to_screen_y(m, y),
				    m->map_surface);

			}
		}
	}

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
	}

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
	if (1 || start_y != prev_start_y || 
	    start_x != prev_start_x ||
	    end_y != prev_end_y ||
	    end_x != prev_end_x ||
	    clip.x != prev_clip_x ||
	    clip.y != prev_clip_y) {

		prev_start_x = start_x;
		prev_start_y = start_y;
		prev_end_x = end_x;
		prev_end_y = end_y;
		prev_clip_x = clip.x;
		prev_clip_y = clip.y;

		blit = 1;

		/* Create a visibility-map */
		SDL_FillRect(m->fog_surface, NULL, 0);
		for (x = 0; x < m->width; x ++) {
			for (y = 0; y < m->height; y ++) {
				if (!map_pos_is_visible(m, p, x, y))
					render_animation_full(rs.fog_tile,
					    0,
					    x * FRAME_WIDTH,
					    y * FRAME_HEIGHT,
					    m->fog_surface);
			}
		}
		/* Clear whole map, we need to move it */
	}

	/* Even if we're not blitting anything, we need to check if any of the
	 * tiles are dirty
	 */
	SDL_Rect tile_rect;
	tile_rect.w = FRAME_WIDTH;
	tile_rect.h = FRAME_HEIGHT;

	clip.x = start_x * FRAME_WIDTH;
	clip.y = start_y * FRAME_HEIGHT;
	clip.w = screen_width * FRAME_WIDTH;
	clip.h = screen_height * FRAME_WIDTH;
	SDL_BlitSurface(m->map_surface, &clip, rs.screen, &screen_rect);
	SDL_BlitSurface(m->fog_surface, &clip, rs.screen, &screen_rect);
	
	render_animation(p->animation_id,
	    map_to_screen_x(m, p->tile_x - start_x) + p->xx,
	    map_to_screen_y(m, p->tile_y - start_y) + p->yy,
	    p->animation_frame);
	    
	return 0;
}
