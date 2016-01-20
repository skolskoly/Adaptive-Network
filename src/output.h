#ifndef OUT_H
#define OUT_H

#include "SDL.h"
#include "macros.h"

struct OUTPUT
{
	SDL_Window * win;
	SDL_Texture * tex;
	SDL_Renderer * ren;
	SDL_Surface * srf;
	SDL_Thread * gfx_thread;
};

void update(OUTPUT * ctx);
OUTPUT * createContext(UI32 w, UI32 h,const char * title);
OUTPUT * freeContext(OUTPUT * ctx);

int outputThread(void *data);

#endif