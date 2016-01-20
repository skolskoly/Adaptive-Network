#include "console.h"

#include "macros.h"
#include "simulation.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h> 

// This is left out of the header as it
// is only a helper structure for getLine()

struct INBUF
{
	char block[16];
	INBUF * next;
};

char * getLine(){	

	// gets a variable length string from the console
	// allocating 16 bytes of characters at a time
	// and then converting them to a string in
	// contiguous memory

	INBUF memory;
	memory.next = NULL;
	INBUF * current = &memory;
	int total = 0;
	char ch = getchar();
	
	while(ch != '\n')
	{
	
		current->block[ total % 16 ] = ch;
		
		if(total % 16 == 0)
		{
			current->next = (INBUF *) malloc(sizeof(INBUF));
			current = current->next;
			current->next = NULL;
		}
		
		total++;
		ch = getchar();
		
	}
	
	// Allocate total+1 to add room for the null terminator
	
	char * str = (char *) malloc(sizeof(char)*(total+1));
	current = &memory;
	
	for(int i = 0; current != NULL && i < total; i++)
	{
		str[i] = current->block[ i % 16 ];
		
		if(i % 16 == 0)
		{
			INBUF * tmp = current;
			current = current->next;
			free(tmp);
		}
		
	}
	
	str[total] = '\0';
	
	return str;

}

UI32 readInt()
{
	char buffer[16];
	fgets(buffer, 16, stdin);
	return atoi(buffer);
}

void wait(SIMULATION * sim)
{
	if(sim != NULL)
	{
		sim->running = false;
		for(UI32 i = 0 ; i < sim->n_threads; i++)
			SDL_WaitThread(sim->sim_threads[i]->thread, NULL);
		SDL_WaitThread(sim->output->gfx_thread, NULL);
	}

}

SIMULATION * kill(SIMULATION * sim)
{
	if(sim != NULL){
	
		wait(sim);
		freeSimulation(sim);
		
		printf("Simulation terminated successfully. \n");
	}
	else
	{
		printf("No active simulation to terminate.\n");
	}
	
	return NULL;
}

SIMULATION * run(SIMULATION * sim)
{
	if(sim != NULL)
		sim = kill(sim);
		
	printf("\nThis a setup for creating a new simulation. Enter 0 for default values.\n\n");
	printf("NOTE: Make sure to run the defaults once to know how much memory the simulation will use.\n");
	printf("	There aren't any checks being done, so be careful not to allocate gigabytes of RAM.\n\n");
	
	
	printf("Enter width (default 128): ");
	UI32 width = readInt();
	if(width == 0) width = 128;
	
	printf("Enter height (default 128): ");
	UI32 height = readInt();
	if(height == 0) height = 128;
	
	printf("Enter the size of the memory blocks (default 32): ");
	UI32 block_size = readInt();
	if(block_size == 0) block_size = 32;
	
	printf("Enter total blocks per cell (default 16): ");
	UI32 n_blocks = readInt();
	if(n_blocks == 0) n_blocks = 16;
	
	printf("Enter number of threads for multi-threading: ");
	UI32 n_threads = readInt();
	n_threads = 1; // multi-threading not implemented
	
	sim = initSimulation(width, height, block_size, n_blocks, n_threads);
	printf("New simulation created successfully.\n");
	
	return sim;

}


