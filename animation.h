#ifndef ANIMATION_H
#define ANIMATION_H
#include "common.h"

#define FRAME_WIDTH 32
#define FRAME_HEIGHT 32

#define SPRITE_SIZE 32

int animation_init();
int animation_load_spritesheet(const char *filename);
int animation_set_spritesheet_blendmode(int spritesheet, int blendmode);

int animation_render_sprite_full(int spritesheet, int spriteid, int x, int y, SDL_Surface *surface);
#define animation_render_sprite(spritesheet, spriteid, x, y) animation_render_sprite_full(spritesheet, spriteid, x, y, rs.screen)
int animation_render_sprite_clipped(int spritesheet, int spriteid, int x, int y, int w, int h, SDL_Surface *surface);

int animation_create(int spritesheet, int start_sprite, int end_sprite);
int animation_next_clip(int animation_id);
int animation_render(int animation_id, int x, int y);
int animation_render_full(int animation_id, int x, int y, SDL_Surface *surface);
int animation_render_clipped(int animation_id, int x, int y, int w, int h, SDL_Surface *surface);
int animation_get_n_frames(int animation_id);
int animation_get_spritesheet_from_anim(int animation_id);
int animation_play(int animation_id, int tx, int ty);
#endif /* end of include guard: ANIMATION_H */
