#
#	This is an automatic build file. Add a .cpp to $(SOURCES)
#	and its associated .h file will be included with the "all" build.
#	I'm not particularly fluent with GNUMake, but I think that this
#	will work with any system.
#
#	SDL2 options:
CSDL= -IC:\mingw_dev_lib\include\SDL2
LSDL= -LC:\mingw_dev_lib\lib -lmingw32 -lSDL2main -lSDL2
#
#		Be sure to change these directories appropriately
#		for your system's copy of SDL.
#
#	IMPORTANT: 
#		always use "int main(int argc, char *argv[])" when linking
#		with SDL. "int main(void)" isn't compatible.
#
#	Note: add -mwindows to CFLAGS to suppress the Windows console.
#
#

CC=g++
CFLAGS= $(CSDL) -std=c++11 -Wall -c -g
LDFLAGS= $(LSDL)
SOURCES= src/main.cpp src/output.cpp src/cell.cpp src/simulation.cpp src/console.cpp src/query.cpp src/macros.h
OBJECTS=$(patsubst src/%.cpp,obj/%.o,$(SOURCES)) 
EXECUTABLE= main

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC)  -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJECTS): | obj

obj:
	@mkdir -p $@
	
obj/%.o : src/%.cpp
	$(CC) $(CFLAGS) $< -o $@