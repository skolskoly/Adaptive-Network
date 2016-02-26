#include "query.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>

QUERY * initQuery()
{

	QUERY * query = (QUERY *) malloc( sizeof(QUERY) );
	query->p_len = 0;
	query->s_len = 1;
	
	updateQuery(query);
	
	query->count = 0;
	
}

void freeQuery(QUERY * query)
{

	if(query->problem)
		free(query->problem);
	if(query->solution)
		free(query->solution);
	free(query);
		
}

void generateProblem(QUERY * query)
{

	for(int i = 0; i < query->q_size; i++)
		query->problem[i] = rand();
	
}

void generateSolution(QUERY * query)
{

	UI32 sum = 0;
	for(int i = 0; i < query->p_len; i++)
		sum += query->problem[i];
	
	*query->answer = sum / len;

}

void updateQuery(QUERY * query)
{
	
	query->count++;
	query->p_len++;
	
	if(query->problem) 
		free(query->problem);
	
	query->problem = malloc(sizeof(char) * p_len);
	
	generateProblem(query);
	generateSolution(query);
		
}
