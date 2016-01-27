#include "simulation.h"

#include "cell.h"
#include "output.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>

int runThread(void * data)
{
	
	// TODO: implement multi-threading
	// Currently the simulation can only handle a single thread.
	// Mutexes are required to deal with cells which try to splice
	// into each other simultaneously in separate threads.
	
	SIM_THREAD * thread = (SIM_THREAD *) data;
	SIMULATION * sim = thread->parent;

	while(sim->running)
	{
		for(UI32 i = 0; i < thread->n_cells && sim->running; i++)
		{
			runCell(thread->first_cell + i, sim);
		}	
	}
	
	return 0;
}

SIM_THREAD * initThread(UI32 size, UI32 start, SIMULATION * sim)
{

	SIM_THREAD * thread = (SIM_THREAD *) malloc(sizeof(SIM_THREAD));
	thread->first_cell = start;
	thread->n_cells = size;
	thread->parent = sim;
	thread->thread = SDL_CreateThread(runThread, "SimulationThread", thread);
	return thread;
	
}

SIMULATION * initSimulation(UI32 width, UI32 height, UI32 block_size, UI32 n_blocks, UI32 n_threads)
{

	SIMULATION * sim = (SIMULATION *) malloc(sizeof(SIMULATION));
	
	{	// Initial simulation values
		sim->running = true;
		sim->block_size = block_size;
		sim->n_cells = width * height;
		sim->n_threads = n_threads;
		sim->fps = 60.0;
		sim->width = width;
		sim->height = height;
	}
		
	{	// Cell initialization
		sim->cells = (CELL **) malloc(sizeof(CELL *)*sim->n_cells);
		for(UI32 i = 0; i < sim->n_cells; i++)
			sim->cells[i] = initCell(block_size, n_blocks);
	}
	
	{	// Cell Thread Initialization
	
		sim->sim_threads = (SIM_THREAD **) malloc(sizeof(SIM_THREAD *)*n_threads);
		
		// This arithmetic is used to evenly divide
		// cells between threads
		
		UI32 quotient = sim->n_cells / n_threads;
		UI32 remainder = sim->n_cells % n_threads;
		UI32 idx = 0;
		UI32 size = 0;
			
		for(UI32 i = 0; i < n_threads; i++)
		{
			idx += size;
			size = quotient + (i < remainder);	
			sim->sim_threads[i] = initThread(size, idx, sim);
		}
	}
	
	sim->window_thread = SDL_CreateThread(windowThread, "WindowThread", (void *) sim);
	
	return sim;
}

void freeSimulation(SIMULATION * sim)
{
	for(UI32 i = 0; i < sim->n_cells; i++)
	{
		freeCell(sim->cells[i]);
	}
	free(sim->cells);
	for(UI32 i = 0; i < sim->n_threads; i++)
	{
		free(sim->sim_threads[i]);
	}
	freeContext(sim->output);
}