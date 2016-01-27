#ifndef SIM_H
#define SIM_H

#include "SDL.h"

#include "cell.h"
#include "output.h"
#include "macros.h"

struct SIMULATION;

struct SIM_THREAD
{

	SIMULATION * parent;
	SDL_Thread * thread;
	UI32 first_cell;
	UI32 n_cells;
	
};

struct SIMULATION
{
	CELL ** cells;
	SIM_THREAD ** sim_threads;
	SDL_Thread * window_thread;
	OUTPUT * output;

	UI32 n_cells;
	UI32 n_threads;
	UI32 block_size;
	
	UI32 width, height;
	
	UI32 running;
	float fps;

};

SIM_THREAD * initThread(UI32 size, UI32 start, SIMULATION * sim);

int runPartial(void * data);

SIMULATION * initSimulation(UI32 width, UI32 height, UI32 block_size, UI32 n_blocks, UI32 n_threads);
void freeSimulation(SIMULATION * sim);

#endif