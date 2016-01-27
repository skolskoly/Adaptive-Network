#ifndef CONSOLE_H
#define CONSOLE_H

#include "output.h"
#include "simulation.h"
#include "macros.h"

char * getLine();
UI32 readInt();

void wait(SIMULATION * sim);
SIMULATION * kill(SIMULATION * sim);
SIMULATION * run(SIMULATION * sim);

#endif