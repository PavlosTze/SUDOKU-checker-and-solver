/********************************************************************
   sudoku.h

   Sudoku solver/generator interface.
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "grid.h"

/* Read a sudoku grid from stdin and return an object Grid_T
   initialized to these values. The input has the format:

1 2 3 4 5 6 7 8 9 
4 5 6 7 8 9 1 2 3
7 8 9 1 2 3 4 5 6
2 3 4 5 6 7 8 9 1 
5 6 7 8 9 1 2 3 4
8 9 1 2 3 4 5 6 7 
3 4 5 6 7 8 9 1 2
6 7 8 9 1 2 3 4 5 
9 1 2 3 4 5 6 7 8 

   Each number is followed by a space. Each line is terminated with
   \n. Values of 0 indicate empty grid cells.
*/
Grid_T sudoku_read(FILE* file);

/* Print the sudoku puzzle defined by g to stream s in the same format
   as expected by sudoku_read(). */
void sudoku_print(FILE *s, Grid_T g);

/* Print all row, col, sub-grid errors/conflicts found in puzzle g;
   some errors may be reported more than once. */
void sudoku_print_errors(Grid_T g);

/* Return true iff puzzle g is correct. */
int sudoku_is_correct(Grid_T g);

/* Sovle puzzle g and return the solved puzzle; if the puzzle has
   multiple solutions, return one of the possible solutions. */
Grid_T sudoku_solve(Grid_T g);

/* Returns true if solution g, as returned by sudoku_solve, has a
   unique choice for each step (no backtracking required). (Note, g
   must have been computed with the use of sudoku_solve.) */
int sudoku_solution_is_unique(Grid_T g);


