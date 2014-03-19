#include <SDL.h>
#include <SDL_image.h>
#include <malloc.h>
#include "animation.h"
#include "sdl.h"

#define INIT_SPRITESHEETS 10
#define INIT_ANIMATIONS 10

struct t_spritesheet {
	SDL_Surface *img;
	SDL_Rect *clips;
	int frames;
};

struct t_animation {
	int spritesheet_id;
	int start;
	int end;
	int curr;
};

struct t_spritesheet **spritesheet_list;
struct t_animation **animation_list;
int animation_count = 0;
int animation_allocated = 0;
int spritesheet_count = 0;
int spritesheet_allocated = 0;

int animation_init()
{
	spritesheet_list = malloc(sizeof(struct t_spritesheet*) *
	    INIT_SPRITESHEETS);
	if (spritesheet_list == NULL) {
		return -1;
	}
	spritesheet_allocated = INIT_SPRITESHEETS;

	animation_list = malloc(sizeof(struct t_animation*) *
	    INIT_ANIMATIONS);
	if (animation_list == NULL) {
		return -1;
	}
	animation_allocated = INIT_ANIMATIONS;

	return 0;
}

int animation_load_spritesheet(const char *filename)
{
	struct t_spritesheet *ss;

	if (spritesheet_count == spritesheet_allocated - 1) {
		spritesheet_list = realloc(spritesheet_list,
		    sizeof(struct t_spritesheet*) * (spritesheet_allocated + INIT_SPRITESHEETS));
		if (spritesheet_list == NULL) {
			perror("realloc(spritesheet_list)");
			return -1;
		}
		spritesheet_allocated += INIT_SPRITESHEETS;
	}
	ss = malloc(sizeof *ss);
	if (ss == NULL) {
		perror("malloc(spritesheet)");
		return -1;
	}

	ss->img = IMG_Load(filename);
	if (ss->img == NULL) {
		return -1;
	}

	SDL_SetSurfaceBlendMode(ss->img, SDL_BLENDMODE_NONE);
	printf("[%d] format: %d\n", animation_count, ss->img->format->format);
	printf("[%d] BPP: %d\n", animation_count, ss->img->format->BitsPerPixel);
	printf("[%d] ByPP: %d\n", animation_count, ss->img->format->BytesPerPixel);
	printf("[%d] RMask: %.8x\n", animation_count, ss->img->format->Rmask);
	printf("[%d] GMask: %.8x\n", animation_count, ss->img->format->Gmask);
	printf("[%d] BMask: %.8x\n", animation_count, ss->img->format->Bmask);
	printf("[%d] AMask: %.8x\n", animation_count, ss->img->format->Amask);
	ss->frames = (ss->img->w / SPRITE_SIZE) * (ss->img->h / SPRITE_SIZE);
	ss->clips = malloc(sizeof(SDL_Rect)*ss->frames);

	int x = 0, y = 0;
	int i;
	for (i = 0; i < ss->frames; i++) {
		ss->clips[i].w = SPRITE_SIZE;
		ss->clips[i].h = SPRITE_SIZE;
		ss->clips[i].x = x;
		ss->clips[i].y = y;
		x += SPRITE_SIZE;
		if (x >= ss->img->w) {
			x = 0;
			y += SPRITE_SIZE;
		}
	}
	spritesheet_list[spritesheet_count] = ss;
	return(spritesheet_count++);
}

int animation_set_spritesheet_blendmode(int spritesheet, int blendmode)
{
	return SDL_SetSurfaceBlendMode(spritesheet_list[spritesheet]->img,
	    blendmode);
}

int
animation_render_sprite_full(int spritesheet, int spriteid, int x, int y, SDL_Surface *surface)
{
	SDL_Rect offset;

	if (spritesheet == -1) return 0;
	if (spriteid == -1) return 0;

	offset.x = x;
	offset.y = y;
	return SDL_BlitSurface(spritesheet_list[spritesheet]->img,
	    &(spritesheet_list[spritesheet]->clips[spriteid]), surface, &offset);
}

int
animation_render_sprite_clipped(int spritesheet, int spriteid, int x, int y, int w, int h, SDL_Surface *surface)
{
	SDL_Rect offset;
	SDL_Rect clip;

	if (spritesheet == -1) return 0;
	if (spriteid == -1) return 0;
	offset.x = x;
	offset.y = y;

	memcpy(&clip, &(spritesheet_list[spritesheet]->clips[spriteid]),
	    sizeof(SDL_Rect));
	clip.w = w;
	clip.h = h;
	
	return SDL_BlitSurface(spritesheet_list[spritesheet]->img, &clip, surface, &offset);
}


int
animation_create(int spritesheet, int start_sprite, int length)
{
	struct t_animation *anim;

	if (animation_count == animation_allocated- 1) {
		animation_list = realloc(animation_list,
		    sizeof(struct t_animation*) * (animation_allocated +
			INIT_ANIMATIONS));
		if (animation_list == NULL) {
			perror("realloc(animation_list)");
			return -1;
		}
		animation_allocated += INIT_ANIMATIONS;
	}
	anim = malloc(sizeof *anim);
	if (anim == NULL) {
		perror("malloc(animation)");
		return -1;
	}

	anim->spritesheet_id = spritesheet;
	anim->start = start_sprite;
	anim->end = start_sprite + length - 1;
	anim->curr = anim->start;

	animation_list[animation_count] = anim;
	return(animation_count++);
}

int
animation_next_clip(int animation_id)
{
	struct t_animation *a;

	if (animation_id == -1) return 0;
	a = animation_list[animation_id];
	a->curr ++;
	if (a->curr > a->end) {
		a->curr = a->start;
		return 1;
	}
	return 0;
}

int
animation_render(int animation_id, int x, int y)
{
	struct t_animation *a;
	
	if (animation_id == -1) return 0;
	a = animation_list[animation_id];
	return animation_render_sprite(a->spritesheet_id, a->curr, x, y);
}

int
animation_render_full(int animation_id, int x, int y, SDL_Surface *surface)
{
	struct t_animation *a;

	if (animation_id == -1) return 0;
	a = animation_list[animation_id];
	return animation_render_sprite_full(a->spritesheet_id, a->curr, x, y,
	    surface);
}

int
animation_render_clipped(int animation_id, int x, int y, int w, int h, SDL_Surface *surface)
{
	struct t_animation *a;

	if (animation_id == -1) return 0;
	a = animation_list[animation_id];
	return animation_render_sprite_clipped(a->spritesheet_id, a->curr,
	    x, y, w, h, surface);
}

int
animation_get_n_frames(int animation_id)
{
	if (animation_id == -1) return 0;
	return(animation_list[animation_id]->end -
	    animation_list[animation_id]->start);
}

int
animation_get_spritesheet_from_anim(int animation_id)
{
	if (animation_id == -1) return -1;
	return animation_list[animation_id]->spritesheet_id;
}

/*
 * animation_play(id, x, y)
 *
 * Add animation to list of animations that should be
 * played, and play it on tile (x,y)
 */
int
animation_play(int animation_id, int tx, int ty)
{
	adv_animation_list *l;

	l = malloc(sizeof *l);
	l->id = animation_id;
	l->x = tx;
	l->y = ty;
	l->next = rs.animation_list;
	rs.animation_list = l;
	return 0;
}
