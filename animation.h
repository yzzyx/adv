#ifndef ANIMATION_H
#define ANIMATION_H
#include "common.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

#define MAX_ANIMATIONS 256
#define FRAME_WIDTH 32
#define FRAME_HEIGHT 32

typedef struct t_animation {
	SDL_Surface *img;
	SDL_Rect *clips;
	int frames;
}animation;

extern animation **animation_list;
int setup_animation();
int load_animation(const char *filename);
int render_animation(int animation_id, int x, int y, int frame);
int animation_get_n_frames(int animation_id);


/*
class animation {
	public:
		SDL_Surface *image;
		SDL_Rect *clips;
		uint32_t frames;
		uint32_t current_frame;
		uint32_t fps;
		uint32_t last_ticks;
		int w, h;

		animation(const char *filename, int frame_width)
		{
			int x;

			this->image = load_image(filename);
			this->current_frame = 0;
			this->frames = this->image->w / frame_width;
			this->clips = new SDL_Rect[frames];
			this->w = frame_width;
			this->h = this->image->h;
			this->fps = 3;
			
			x = 0;
			for (uint32_t i = 0; i < frames; i++) {
				this->clips[i].w = frame_width;
				this->clips[i].h = this->image->h;
				this->clips[i].x = x;
				this->clips[i].y = 0;
				x += frame_width;
			}

		}

		~animation() {
			SDL_FreeSurface(this->image);
			delete this->clips;
		}

		int render(render_space *rs, int x, int y) {
				SDL_Rect offset;

				offset.x = x;
				offset.y = y;

				if (SDL_GetTicks() - last_ticks > 1000 / fps) {
					current_frame ++;
					last_ticks = SDL_GetTicks();
					if (current_frame == frames)
						current_frame = 0;
				}

				return SDL_BlitSurface(image, &clips[current_frame], rs->screen, &offset);
			}

};
*/
#endif /* end of include guard: ANIMATION_H */
