#include <malloc.h>
#include "animation.h"
#include "sdl.h"

animation **animation_list;
int animation_count = 0;

int setup_animation()
{
	animation_list = malloc(sizeof(animation*) * MAX_ANIMATIONS);
	if (animation_list == NULL) {
		return -1;
	}
	return 0;
}

int load_animation(const char *filename)
{
	animation *anim;
	SDL_Surface *tmp;

	anim = malloc(sizeof(animation));

	anim->img = NULL;

	tmp = IMG_Load(filename);
	if (tmp == NULL) {
		return -1;
	}

	anim->img = SDL_DisplayFormatAlpha(tmp);
	SDL_FreeSurface(tmp);

	anim->frames = anim->img->w / FRAME_WIDTH;
	anim->clips = malloc(sizeof(SDL_Rect)*anim->frames);

	int x = 0;
	int i;
	for (i = 0; i < anim->frames; i++) {
		anim->clips[i].w = FRAME_WIDTH;
		anim->clips[i].h = anim->img->h;
		anim->clips[i].x = x;
		anim->clips[i].y = 0;
		x += FRAME_WIDTH;
	}
	animation_list[animation_count] = anim;
	return(animation_count++);
}

int
render_animation(int animation_id, int x, int y, int frame)
{
	SDL_Rect offset;

	if (animation_id == -1) return 0;
	offset.x = x;
	offset.y = y;
	return SDL_BlitSurface(animation_list[animation_id]->img,
	    &(animation_list[animation_id]->clips[frame]), rs.screen, &offset);
}

int render_animation_full(int animation_id, int frame, int x, int y, SDL_Surface *surface)
{
	SDL_Rect offset;

	if (animation_id == -1) return 0;
	offset.x = x;
	offset.y = y;
	return SDL_BlitSurface(animation_list[animation_id]->img,
	    &(animation_list[animation_id]->clips[frame]), surface, &offset);

}

int animation_get_n_frames(int animation_id)
{
	return(animation_list[animation_id]->frames);
}
