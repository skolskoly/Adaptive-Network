#include "SDL.h"

#include "simulation.h"
#include "output.h"
#include "macros.h"
#include "console.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{

	srand(time(NULL));
	SDL_Init(SDL_INIT_VIDEO);
	SIMULATION * sim = NULL;
	bool running = true;
	char * input;
	
	printf("This CLI is a temporary interface. A more fully featured version is in progress. Type \"help\" for a list of commands. \n");
	
	while(running)
	{
		input = getLine();
		
		if( strcmp(input, "run") == 0)
		{
			sim = run(sim);
		} 
		else if( strcmp(input, "kill") == 0)
		{
			sim = kill(sim);
		}
		else if( strcmp(input, "quit") == 0)
		{
			running = false;
		}
		else if( strcmp(input, "help") == 0)
		{

			printf("Commands:\n");
			printf("	run 		- start a simulation\n");
			printf("	kill		- stop the active simulation\n");
			//printf("	pause		- pause simulation\n"); (not implemented)
			//printf("	save		- save simulation to disk\n"); (not implemented)
			//printf("	load		- load simulation from disk\n"); (not implemented)
			//printf("	scale		- scale the output window by an integer factor\n"); (not implemented)
			printf("	quit		- exit program\n");
			printf("	help		- print this help menu\n");
			
			printf("The CLI doesn't yet support command arguments, so \"simulate\" uses a guided setup for the time being. Some tools for run-time variable access will be added as well. \n");
			
		}
		else
		{
			printf("Command not recognized.\n");
		}
	
		free(input);
	}
	
	wait(sim);
	
	// Remaining heap memory freed by OS
	
	return 0;
}
