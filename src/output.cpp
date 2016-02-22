#include "output.h"

#include "SDL.h"
#include "simulation.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>

OUTPUT * createContext(UI32 w, UI32 h, const char * title)
{
	OUTPUT * ctx = (OUTPUT *) malloc(sizeof(OUTPUT));
	
	UI32 rmask = 0x000000FF;
	UI32 gmask = 0x0000FF00;
	UI32 bmask = 0x00FF0000;
	UI32 x = 200;
	UI32 y = 100;
	
	ctx->srf = SDL_CreateRGBSurface(0, w, h, 24, rmask, gmask, bmask, 0);
	ctx->win = SDL_CreateWindow(title,x,y, w, h, SDL_WINDOW_SHOWN);
	ctx->ren = SDL_CreateRenderer(ctx->win, -1, SDL_RENDERER_SOFTWARE);
	ctx->tex = SDL_CreateTexture(ctx->ren, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, w, h); 
	ctx->fps = 60.0;
	
	return ctx;
}

void update(OUTPUT * ctx)	
{
    SDL_UpdateTexture(ctx->tex, NULL, ctx->srf->pixels, ctx->srf->pitch);
	SDL_RenderClear(ctx->ren);
    SDL_RenderCopy(ctx->ren, ctx->tex, NULL, NULL);
    SDL_RenderPresent(ctx->ren);
}

OUTPUT * freeContext(OUTPUT * ctx)
{
	SDL_FreeSurface(ctx->srf);
	SDL_DestroyWindow(ctx->win);
	SDL_DestroyRenderer(ctx->ren);
	SDL_DestroyTexture(ctx->tex);
	free(ctx);
	
	return ctx;
}

int windowThread(void * data)
{
	// Yes SDL requires I launch a thread in a thread
	// just to handle events. 
	// IF YOU EVER WRITE AN API NEVER DO THI-
	
	SIMULATION * sim = (SIMULATION *) data;
	sim->output = createContext(sim->width,sim->height,"Adaptive Network");
	sim->output->gfx_thread = SDL_CreateThread(outputThread, "GraphicsThread", (void *) sim);
	
	while(sim->running)
	{
		for(SDL_Event e; SDL_PollEvent(&e) != 0;)
		{
			if(e.quit.type == SDL_QUIT)
			{// Checks if the X button has been pressed.
				sim->running = false;
			}
		}
	}
	
	
	
	return 0;
	
}

int outputThread(void * data)
{
	SIMULATION * sim = (SIMULATION *) data;
	
	SDL_Surface * srf = sim->output->srf;
	UI8 * rgb_surface = (UI8 *) sim->output->srf->pixels;
	
	while(sim->running)
	{
		UI32 ticks = SDL_GetTicks();
		
		for(UI32 x = 0; x < sim->width; x++)
		{
			for(UI32 y = 0; y < sim->height; y++)
			{
				UI32 cel_idx = x + y * sim->width;
				
				if(sim->cells[cel_idx]->lock == false){
					for(UI32 c = 0; c < 3; c++)
					{
						// Iterate across every cell and copy its RGB
						// values to the screen buffer
						
						UI8 color = sim->cells[cel_idx]->color[c];
						rgb_surface[ 3*x + y*srf->pitch + c ] = color;
					}
				}
			}
		}
		
		update(sim->output); // Update the display with the new pixel buffer
		
		ticks = SDL_GetTicks() - ticks;
		if(ticks < 1000/sim->output->fps) SDL_Delay(1000/sim->output->fps - ticks);
	}
	return 0;
}


