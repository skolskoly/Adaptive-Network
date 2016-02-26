#ifndef QUERY_H
#define QUERY_H

#include "macros.h"

struct QUERY
{
	UI8 * problem;
	UI8 * solution;
	
	UI32 p_len;
	UI32 s_len;
	
	UI32 count;
};

QUERY * initQuery();
void freeQuery(QUERY * query);

void generateProblem(QUERY * query);
void generateSolution(QUERY * query);

void updateQuery(QUERY * query);



#endif