#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "grid.h"

/* update value of i,j to n */
void grid_update_value(Grid_T *g, int i, int j, int n)
{
	assert(g != NULL);
	assert(n >= 0 && n < 10);
	g->elts[i][j].val = n;
}


/* return value of i,j */
int grid_read_value(Grid_T g, int i, int j)
{
	return g.elts[i][j].val;
}

/* set to 1 choice n for elt i,j */
void grid_set_choice(Grid_T *g, int i, int j, int n)
{
	assert(g != NULL);
	assert(n >= 0 && n < 10);	
	g->elts[i][j].choices.num[n] = 1;
}

/* set to 0 choice n for elt i,j */
void grid_clear_choice(Grid_T *g, int i, int j, int n)
{
	assert(n >= 0 && n < 10);
	g->elts[i][j].choices.num[n] = 0;
}

/* true if choice n for elt i,j is valid */
int grid_choice_is_valid(Grid_T g, int i, int j, int n)
{
	assert(n >= 0 && n < 10);
	return g.elts[i][j].choices.num[n];
}

/* remove n from choices of elt i,j and adjust count only if n is a
   valid choice for elt i,j */
void grid_remove_choice(Grid_T *g, int i, int j, int n)
{
	assert(g != NULL);
	assert(n >= 0 && n < 10);
	if(g->elts[i][j].choices.num[n])
	{
		g->elts[i][j].choices.num[n] = 0;
		g->elts[i][j].choices.count--;
	}
}

/* return count, set (to 9), or clear (to 0) count for elt i, j */
int grid_read_count(Grid_T g, int i, int j)
{
	return g.elts[i][j].choices.count;
}

/* set to 9 count for elt i, j */
void grid_set_count(Grid_T *g, int i, int j)
{
	assert(g != NULL);
	g->elts[i][j].choices.count = 9;
}

/* set to 0 count for elt i, j */
void grid_clear_count(Grid_T *g, int i, int j)
{
	assert(g != NULL);
	g->elts[i][j].choices.count = 0;
}

/* return unique flag for g */
int grid_read_unique(Grid_T g)
{
	return g.unique;
}

/* set to 1 unique flag for g */
void grid_set_unique(Grid_T *g)
{
	assert(g != NULL);
	g->unique = 1;
}

/* set to 0 unique flag for g */
void grid_clear_unique(Grid_T *g)
{
	assert(g != NULL);
	g->unique = 0;
}





