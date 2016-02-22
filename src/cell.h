#ifndef CELL_H
#define CELL_H

#include "macros.h"
#include "SDL.h"

struct SIMULATION;

struct BLOCK
{
	UI16 * memory;
	BLOCK * next;
	UI32 size;
};

struct CELL
{
	UI16 * ip;
	
	BLOCK * start;
	BLOCK * prog_block;
	BLOCK * data_block;
	
	UI32 n_blocks;
	UI32 free_memory;
	
	UI16 reg[8];
	UI8 color[3];
	UI8 lock;
};

enum OPERATIONS
{
	ADI,
	ADD,
	JEQ,	
	MEM,
	BTW,
	SYS,
	N_OPS
};

enum MEM_OPS
{
	M_READ,
	M_WRITE,
	M_NEXT,
	M_START,
	N_M_OPS
};

enum BTW_OPS
{
	B_AND,
	B_NOT,
	B_NAND,
	B_OR,
	B_XOR,
	B_LSH,
	B_RSH,
	N_B_OPS
};

enum SYS_OPS
{
	S_OUT,
	S_SPLICE,
	S_FRY,
	N_S_OPS
};

CELL * initCell(UI32 block_size, UI32 n_blocks);
int runCell(UI32 cel_idx, SIMULATION * sim);
void freeCell(CELL * c);

#endif