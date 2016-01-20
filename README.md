# Adaptive-Network

This program simulates a network of Turing machines. The machines are capable of interacting by copying the content of their processes and data into one another. On the screen, these machines are represented as an array of pixels whose colours mutate whenever one machine copies to another. The intent of this project is to create a simulation of a competitive evolutionary processes.

This is not the first implementation of the program, and there are a number of features yet to be implemented. The machines do not behave exactly as they had in the past, and there are some modifications that need to be made to the structure of their memory in order to make their instruction set more powerful. For the time being, the implementation exists as complete proof of concept.

In terms of utility features, pausing and resizing the simulation are going to implemented very shortly. There are also some visualizations modes that need to be added.

On the other hand, I have a large number of additional instructions I'd like to add to the machines, such as the ability to observe colour values, and receive input and send output to the user.

Here is information for the source files. Scroll down to the cell files to see the instruction set implementation.

####MAIN																
<div align="right">
	CPP/H
</div>
</br>

- Sets up a command line interface to launch and control simulations.
	- type "help" for a list of commands.

</br>
####MACROS	
<div align="right">
	H
</div>
</br>


- Defines UI32, UI16, and UI8 as macros for unsigned int types.
	- this is just a style preference for consistency when dealing with raw memory.
	- nobody wants to type "unsigned short int."
	- UI8 is used for byte values, char is used for text data. 

</br>
####SIMULATION

<div align="right">
	CPP/H
</div>
</br>


- createSimulation() sets up simulation variables, allocates resources and launches threads.
- Multi-threading is not yet implemented, so n_threads is always 1 for now.
- Handles SDL Events.
	- SDL requires UI events to be handled in the thread that created the window, so this must be done here.

</br>
####CELL
<div align="right">
	CPP/H
</div>													
</br>	
															

A cell is a structure which is represented by one pixel on the screen.
Each cell has a pointer to linked blocks of memory, and eight virtual registers.
An index is stored which is incremented each time an instruction is read from
memory, which allows the cell to simulate a simple virtual machine.

The instructions are intended to be Turing complete, allowing for a cell to perform
any meaningful computation, however the implementation that has been chosen 
may not prove to be the most efficient, and is subject to change.

The crux of the simulation is the ability of a cell to copy or splice its own
instructions into another cell, in order to simulate an evolutionary process. When a cell passes its own
code into a target cell, the target will take the source color with a small mutation.
This allows us to visualize different strains of code as they propagate like bacteria.

The memory of a cell is composed of blocks of UI16 values which are linked together by pointers.
These lists makes it more efficient to splice blocks together, or append new blocks to the cells. 
Originally, all of a cell's memory would need to be reallocated in order to compensate for changes in length. 
This was obviously very slow, as every value must be copied over to the new array. Linked lists aren't the only 
possible implementation for memory, either; it's just what has been implemented in this version.

Another thing to note, is that the final block in the list links back to the first, so a
cell's code will loop infinitely.

- runCell() runs a single CPU instruction for the cell passed to it.
- instructions are stored as UI16s, which are encoded with this format:


| 	000		|	 000	|	 000	|	 000	|	 0000	|
| --------- | --------- | --------- | --------- | --------- |
|	 REG1	|	 REG2	|	 REG3	|	 REG4	|	 OP		|

the "REG" values are 3 bit indexes which refer to one of the 8 CPU registers,
while the "OP" field specifies one of up to 16 commands.
Sometimes the REG fields are used to specify sub operations, rather than registers,
and in other cases the immediate values of the 3 bit integers are used in arithmetic.


|	NAME	|	Description									|
| ----------|:--------------------------------------------|
| | |
|	<strong>ADD</strong>		|	<strong>Ordinary addition</strong>							|
|			|	Take the value stored in REG2, add REG3, store the result into REG1	|
|	<strong>ADI</strong>		|	<strong>Add immediate value</strong>							|
|			|	Cast REG2 and REG3 to a single byte, don't treat this as a register, just add it immediately to REG1.	|
|	<strong>JEQ</strong>		|	<strong>Jump if equal</strong>								|
|			|	If the two values in REG1 and REG2 are equal, jump forwards by the value stored in REG3	|
|	<strong>MEM</strong>		|	<strong>Memory operations</strong>							|
|			| 	REG3 specifies a sub-operation, read or write. REG2 specifies some location in cell memory that will either be read or written to REG1. |
|	<strong>BTW</strong>		|	<strong>Bitwise operations</strong>							|
|			| 	REG3 specifies the type of bitwise operation. Beyond that, they all work the same as addition; REG2 and REG3 are operated on, and then stored to REG1.	|
|	<strong>SYS</strong>		|	<strong>System Calls</strong>								|
|			|	These are extended operations handled by the system. REG values are treated as op codes, rather than registers.	|

The only system operation that is currently implemented is "splice," which simply
extracts a block of memory from the source cell, and puts it into the target cell.


</br>
####OUTPUT																	
<div align="right">
	CPP/H
</div>
</br>

- outputThread() is responsible for putting cell pixels onto the screen.
- SDL is being used for platform independent graphics.

Consult the SDL API for more specific information: https://wiki.libsdl.org/CategoryAPI

