#include "cell.h"

#include "simulation.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

UI8 bitMask(UI16 value, UI8 shift, UI8 bits)
{
	return (value >> shift)&( (0x01 << bits) - 1);
}

UI16 * getMemoryPointer(CELL * c, UI32 index, SIMULATION * sim)
{
	// Walk through the cell's memory blocks
	// and retrieve a UI16 pointer

	BLOCK * tmp = c->blocks;
	index %= c->n_blocks * sim->block_size;
	
	while( index >= sim->block_size )
	{
		tmp = tmp->next;
		index -= sim->block_size;
	}
	
	return &tmp->memory[index];
	
}

void nextInstruction(CELL * c, SIMULATION * sim)
{
	// Increments the cells instruction pointer
	// accounting for the fact that the pointer
	// will need to jump to the next block
	// when it goes out of bounds.

	c->ip++;
	UI32 index = c->ip - c->blocks->memory;
	if( index >= sim->block_size)
	{
		c->blocks = c->blocks->next;
		c->ip = c->blocks->memory;
	}
}

void fmtInstruction(CELL * c, UI8 * reg, UI8 * opcode)
{
	// Copies the value at the instruction pointer
	// into a form that can be processed by runCell()

	reg[0] = bitMask(*c->ip, 4, 3);
	reg[1] = bitMask(*c->ip, 7, 3);
	reg[2] = bitMask(*c->ip, 10,3);
	reg[3] = bitMask(*c->ip, 13,3);
	*opcode = bitMask(*c->ip, 0, 4) % N_OPS;
}

void outputInstruction(UI16 * inst)
{
	const char * op;
	switch(inst[4])
	{
		case ADI: op = "ADI"; break;
		case ADD: op = "ADD"; break;
		case JEQ: op = "JEQ"; break;
		case MEM: op = "MEM"; break;
		case BTW: op = "BTW"; break;
		case SYS: op = "SYS"; break;
		default:  op = "NUL"; break;
	}
	
	printf("%s %i , %i , %i , %i\n", op, inst[0], inst[1], inst[2], inst[3]);
}

BLOCK * initBlock(UI32 block_size)
{

	BLOCK * block = (BLOCK *) malloc(sizeof(BLOCK));
	block->memory = (UI16 *) malloc(sizeof(UI16)*block_size);
	for(UI32 j = 0; j < block_size; j++)
		block->memory[j] = rand();
		
	return block;
}

CELL * initCell(UI32 block_size, UI32 n_blocks)
{
	
	CELL * c = (CELL *) malloc(sizeof(CELL));
	
	{	// List Memory Allocation
		c->blocks = initBlock(block_size);
		BLOCK * tmp = c->blocks;
		for(UI32 i = 1; i < n_blocks; i++)
		{
			tmp->next = initBlock(block_size);;
			tmp = tmp->next;
		}
		tmp->next = c->blocks;
	}
	
	{	// Initial Values
		c->lock = false;
		c->n_blocks = n_blocks;
		c->ip = c->blocks->memory;
		for(UI32 i = 0; i < 8; i++) c->reg[i] = rand();
		for(UI32 i = 0; i < 3; i++) c->color[i] = rand();
	}
	
	return c;

}

void freeCell(CELL * c)
{
	BLOCK * tmp = c->blocks; 
	for(UI32 i = 0; i < c->n_blocks; i++)
	{
		tmp = tmp->next;
		free(c->blocks->memory);
		free(c->blocks);
		c->blocks = tmp;
	}
	free(c);
}

BLOCK * extractBlock(CELL * c, UI32 idx)
{

	// Extracts a block from the cell
	// stepping through the blocks
	// idx number of times
	// severing its link by rejoining 
	// the block behind with the 
	// block ahead of it

	idx %= c->n_blocks;
	BLOCK * block = c->blocks;
	for(UI32 i = 0; i < idx; i++)
		block = block->next;
		
	BLOCK * tmp = block;
	block = block->next;
	
	if(block == c->blocks)
	{
		c->blocks = c->blocks->next;
		c->ip = c->blocks->memory;
	}
	
	tmp->next = block->next;
	
	c->n_blocks--;
	
	return block;
}

void insertBlock(CELL * c, BLOCK * block, UI32 idx)
{
	if(c->n_blocks == 0)
	{
	
		// If there are no blocks in the target
		// cell, we need to make sure the inserted block
		// links to itself to ensure the program loops
	
		block->next = block;
		c->blocks = block;
		c->ip = block->memory;
	}
	else
	{
		block->next = c->blocks->next;
		c->blocks->next = block;
	}
	
	c->n_blocks++;
	
}

void mutateColor(CELL * src, CELL * dst)
{
	dst->lock = true;
	for(UI8 c = 0; c < 3; c++)
	{	
		dst->color[c] = src->color[c] -1 + rand()%3;
		
		// Check edge cases to prevent overflow
		// (It looks odd to see bright colours
		//	flip to black in one iteration)
		
		if(dst->color[c] == 0xFF) dst->color[c]--;
		if(dst->color[c] == 0x00) dst->color[c]++;
	}

	dst->lock = false;
}

UI32 getNeighbour(UI32 src_idx, UI16 val, SIMULATION * sim)
{
	// Use a register value and derive an index
	// that is one cell away from src_idx

	int src_x = src_idx % sim->width;
	int src_y = src_idx / sim->width;
	
	int x = src_x + bitMask(val,0,8)%3 -1;
	int y = src_y + bitMask(val,8,8)%3 -1;
	
	x = x % sim->width;
	y = y % sim->height;

	return x + y*sim->width;
}

int runCell(UI32 cel_idx, SIMULATION * sim)
{
	CELL * c = sim->cells[cel_idx];
	
	if(c->n_blocks == 0) return 0; // Avoid processing an inactive cell
	
	UI8 rg_idx[4];
	UI8 opcode;
	
	fmtInstruction(c, rg_idx, &opcode);
	
	// This section uses the UI16 taken from the cell's
	// IP (instruction pointer) and performs some
	// virtual CPU operation. Consult the readme for
	// detailed information about the instructions.
	
	switch(opcode)
	{
		case ADI:
			{
				UI8 shift = bitMask(rg_idx[1],2,1) * 8; 
				UI8 inc = bitMask(rg_idx[1],0,2);
				inc += rg_idx[2] << 2;
				inc += rg_idx[3] << 5;
				c->reg[ rg_idx[0] ] += inc << shift;
			}
		break;
		
		case ADD:
			c->reg[ rg_idx[0] ] += c->reg[ rg_idx[1] ] + c->reg[ rg_idx[2] ] + (1 << rg_idx[3]);
		break;
		
		case JEQ:
			if(c->reg[ rg_idx[0] ] == c->reg[ rg_idx[1] ])
			{
				UI32 new_idx = c->ip - c->blocks->memory;
				new_idx += c->reg[ rg_idx[2] ] + rg_idx[3];
				c->ip = getMemoryPointer(c, new_idx, sim);
			}
		break;
		
		case MEM:
		{
			UI32 mem_idx = c->reg[ rg_idx[1] ] + rg_idx[2];
			UI16 * memory = getMemoryPointer(c, mem_idx, sim);
			
			switch(rg_idx[3] % N_M_OPS)
			{
				case M_READ:
					c->reg[ rg_idx[0] ] = *memory;
				break;
				case M_WRITE:
					*memory = c->reg[ rg_idx[0] ]; 
				break;
			}
			
		}break;
		
		case BTW:
			switch(rg_idx[3] % N_B_OPS)
			{
				case B_AND:
					c->reg[ rg_idx[0] ] = c->reg[ rg_idx[1] ] & c->reg[ rg_idx[2] ];
				break;
				case B_OR:
					c->reg[ rg_idx[0] ] = c->reg[ rg_idx[1] ] | c->reg[ rg_idx[2] ];
				break;
				case B_XOR:
					c->reg[ rg_idx[0] ] = c->reg[ rg_idx[1] ] ^ c->reg[ rg_idx[2] ];
				break;
				case B_NAND:
					c->reg[ rg_idx[0] ] = ~(c->reg[ rg_idx[1] ] & c->reg[ rg_idx[2] ]);
				break;
				case B_NOT:
					c->reg[ rg_idx[0] ] = ~c->reg[ rg_idx[1] ];
				break;
				case B_LSH:
					c->reg[ rg_idx[0] ] = c->reg[ rg_idx[1] ] << c->reg[ rg_idx[2] ];
				break;
				case B_RSH:
					c->reg[ rg_idx[0] ] = c->reg[ rg_idx[1] ] >> c->reg[ rg_idx[2] ];
				break;			
			}
		break;
		
		case SYS:
			switch( rg_idx[0] % N_S_OPS)
			{
				case S_OUT:
					//printf("REG:\t%i ,\t\n%i , \t\n%i , \t\n%i\n", c->reg[0],c->reg[1],c->reg[2],c->reg[3]);
				break;
				case S_SPLICE:
				{
					
					UI32 dst_idx = getNeighbour(cel_idx, c->reg[0], sim);
					CELL * dst = sim->cells[dst_idx];
					
					BLOCK * block = extractBlock(c, c->reg[1] );			
					insertBlock(dst, block, 0);
					
					mutateColor(c, dst);
										
				}break;
			}
		break;
		
		default:
		break;
	}
	
	nextInstruction(c, sim);
	
	return 0;
}
