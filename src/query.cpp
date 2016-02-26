#include "query.h"
#include "macros.h"

#include <stdio.h>
#include <stdlib.h>

QUERY * initQuery()
{

	QUERY * query = (QUERY *) malloc( sizeof(QUERY) );
	query->p_len = 1;
	query->s_len = 1;
		
	updateQuery(query);
		
	query->count = 0;
	
	return query;
	
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

	for(UI32 i = 0; i < query->p_len; i++)
		query->problem[i] = rand();
	
}

void generateSolution(QUERY * query)
{

	UI32 sum = 0;
	for(UI32 i = 0; i < query->p_len; i++)
		sum += query->problem[i];
	
	*query->solution = sum / query->p_len;

}

void updateQuery(QUERY * query)
{
	
	if(query->problem) 
		free(query->problem);
	if(query->solution) 
		free(query->solution);
	
	query->problem = (UI8 *) malloc(sizeof( UI8 ) * query->p_len);
	query->solution = (UI8 *) malloc(sizeof( UI8 ) * query->s_len);
	
	generateProblem(query);
	generateSolution(query);
	
	query->count++;
	query->p_len++;
	
}
