#include <assert.h>
#include <ctype.h>
#include "sudoku.h"
 
#define MAXSIZE 9
#define SUBGRIDSIZE 3

/*
	fix all choices for every element in soduku Grid
 */
static void sudoku_init_choices(Grid_T *g)
{
	int rowCounter,columnCounter,k,l,value;
	grid_set_unique(g);
	
	/* for each element set its choice as valid = 1 */
    for (rowCounter = 0; rowCounter < MAXSIZE; rowCounter++) {
		for (columnCounter = 0; columnCounter < MAXSIZE; columnCounter++){
			grid_set_count(g, rowCounter, columnCounter);
			grid_clear_choice(g, rowCounter, columnCounter, 0);
			for (value = 1; value < MAXSIZE + 1; value++) {
				grid_set_choice(g, rowCounter, columnCounter, value);
			}
		  }
		}
	
	/* then check the other elements at the same row or column */
	for (rowCounter = 0; rowCounter < MAXSIZE; rowCounter++) {
		for (columnCounter = 0; columnCounter < MAXSIZE; columnCounter++){
			value = grid_read_value(*g, rowCounter, columnCounter);
			if (value != 0) {		/* if cell is not empty, clear its count and all of its choices */
				grid_clear_count(g, rowCounter, columnCounter);
			for (value = 1; value < MAXSIZE + 1; value++) {
				grid_clear_choice(g, rowCounter, columnCounter, value);
			}
			continue;
			}
			/* if cell is empty we get here */
			for (k = 0; k < MAXSIZE; k++) {
				value = grid_read_value(*g, rowCounter, k);
				grid_remove_choice(g, rowCounter, columnCounter, value);		/* clear all the choices that already exist in the same row */
				value = grid_read_value(*g, k, columnCounter);
				grid_remove_choice(g, rowCounter, columnCounter, value);		/* clear all the choices that already exist in the same column */
			}
	
	/* then check the other elements at the same substring and clear all the choices that already exist in that */		
	for (k = (rowCounter/SUBGRIDSIZE)*SUBGRIDSIZE; k < (rowCounter/SUBGRIDSIZE)*SUBGRIDSIZE+SUBGRIDSIZE; k++) {
        for (l = (columnCounter/SUBGRIDSIZE)*SUBGRIDSIZE; l < (columnCounter/SUBGRIDSIZE)*SUBGRIDSIZE+SUBGRIDSIZE; l++) {
			value = grid_read_value(*g, k, l);
			grid_remove_choice(g, rowCounter, columnCounter, value);
        }
      }
    }
  }
  return;
}

/* take as input a sudoku "g" and check for a unique choice for a cell, if found return one of them
   otherwise return 0 */
static int sudoku_try_next(Grid_T g, int *row, int *col)
{
	int rowCounter,columnCounter,value,count;
	int choices = MAXSIZE + 1;
	
	/* for each element */
	for(rowCounter = 0; rowCounter < MAXSIZE; rowCounter++)
	{
		for(columnCounter = 0; columnCounter < MAXSIZE; columnCounter++)
		{
			count = grid_read_count(g,rowCounter,columnCounter);	/* read the count of choices at each element */
			if(count > 0 && count < choices)		/* if it is valid and less than someone's else element's choices then set choices = count */
			{
				choices = count;
			}
		}
	
	}
	
	while(choices < MAXSIZE + 1)		/* choices must be the lowest we found */
	{
		rowCounter = rand()%MAXSIZE;
		columnCounter = rand()%MAXSIZE;
		count = grid_read_count(g,rowCounter,columnCounter);		/* take random cells until count = choices */
		if(count == choices)
		{
			while(1)
			{
				value = rand()%MAXSIZE + 1;		/* random values until find someone that is valid and put it and return it */
				if(grid_choice_is_valid(g,rowCounter,columnCounter,value))
				{
					*row = rowCounter;
					*col = columnCounter;
					return value;
				}
			}
		}
	}
	
	return 0;
}

/* remove from the grid "g" the choice "n" from the element with row "i" and column "j" 
    and return the number of choices that we had for this element 
*/
static int sudoku_update_choice(Grid_T *g, int i, int j, int n)
{
	int choices = grid_read_count(*g,i,j);	
	grid_remove_choice(g,i,j,n);
	
	return choices;
}

/* eliminate choice "n" from all the elements at row "r", column "c" and at the appropriate substring */
static void sudoku_eliminate_choice(Grid_T *g, int r, int c, int n)
{
  int i, j;
  for (i = 0; i < MAXSIZE; i++) {
    grid_remove_choice(g, r, i, n);		/* remove the choice from the elements at the same row */
    grid_remove_choice(g, i, c, n);		/* remove the choice from the elements at the same column */
  }
  
  /* compute the substring and remove the choice of the elements at the same substring */
  for (i = (r/SUBGRIDSIZE)*SUBGRIDSIZE; i < (r/SUBGRIDSIZE)*SUBGRIDSIZE+SUBGRIDSIZE; i++) {
    for (j = (c/SUBGRIDSIZE)*SUBGRIDSIZE; j < (c/SUBGRIDSIZE)*SUBGRIDSIZE+SUBGRIDSIZE; j++) {
      grid_remove_choice(g, i, j, n);
    }
  }
  return;
}

/* apply the choice "n" at our sudoku grid "g" at element with row "i", column "j" */
static void sudoku_apply_choice(Grid_T *g, int i, int j, int n)
{
  grid_update_value(g, i, j, n);
  return;
}

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
Grid_T sudoku_read(FILE* file)
{
	Grid_T sudoku;
	int rowCounter = 0;
	int columnCounter = 0;
	int counter = 0;
	int number;
	char input[2];
	input[1] = '\n';
	while((input[0] = fgetc(file)) != EOF )		/* until the end of the file */
	{
		if (input[0]=='\n')
        {
            rowCounter++;
            columnCounter = 0;
        } else if(isdigit((int)input[0]))		/* if we read a value */
		{
			number = atoi(input);
			if(number > MAXSIZE || number < 0)
			{
				fprintf(stderr, "Wrong input, number must be 0-9");
				exit(EXIT_FAILURE);
			} else		/* if it is valid, put the value at our grid */
			{
				grid_update_value(&sudoku,rowCounter,columnCounter,number);
				columnCounter++;
				counter++;
			}
		}
	}
	
	if(counter != MAXSIZE*MAXSIZE)
	{
		fprintf(stderr,"Wrong input, more or less numbers that needed, our grid-sudoku must have 81 cells");
		exit(EXIT_FAILURE);
	}
	return sudoku;
}


/* Print the sudoku puzzle defined by g to stream s in the same format
   as expected by sudoku_read(). */
void sudoku_print(FILE *s, Grid_T g)
{
	int i,j;
	for(i = 0; i < MAXSIZE; i++)
	{
		for(j = 0; j < MAXSIZE; j++)
		{
			fprintf(s, "%d ", grid_read_value(g,i,j));
		}
		fprintf(s, "\n");
	}
	return;
}


/* Print all row, col, sub-grid errors/conflicts found in puzzle g;
   some errors may be reported more than once. */
void sudoku_print_errors(Grid_T g)
{
	int rowCounter,columnCounter,k,l,value;
	for(rowCounter = 0; rowCounter < MAXSIZE; rowCounter++)				/* for each row */
	{
		for(columnCounter = 0; columnCounter < MAXSIZE; columnCounter++)			/* for each column */
		{
			value = grid_read_value(g,rowCounter,columnCounter);
			if(value == 0)			/* if we have a 0 then sudoku is not correct so return 0*/
			{
				printf("Error in element: (%d , %d) , it has value 0\n",rowCounter,columnCounter);
			}
			for(k = 0; k < MAXSIZE; k++)
			{
				if(k != columnCounter && value == grid_read_value(g,rowCounter,k) && value != 0)	/* check elements at row */
				{
					printf("Error in row %d at value %d in elements: (%d , %d) , (%d , %d) \n",rowCounter,value,rowCounter,columnCounter,rowCounter,k);
				}
				if(k != rowCounter && value == grid_read_value(g,k,columnCounter) && value != 0)	/* check elements at column */
				{
					printf("Error in column %d at value %d in elements: (%d , %d) , (%d , %d) \n",columnCounter,value,rowCounter,columnCounter,k,columnCounter);
				}
			}
			
			/* compute each substring and check it */
			for(k = (rowCounter/SUBGRIDSIZE) * SUBGRIDSIZE; k < (rowCounter/SUBGRIDSIZE) * SUBGRIDSIZE + SUBGRIDSIZE; k++)
			{
				for(l = (columnCounter/SUBGRIDSIZE) * SUBGRIDSIZE; l < (columnCounter/SUBGRIDSIZE) * SUBGRIDSIZE + SUBGRIDSIZE; l++)
				{
					if(k != rowCounter && l != columnCounter && value == grid_read_value(g,k,l) && value != 0)
					{
						printf("Error in substring at elements: (%d , %d) , (%d , %d) \n",rowCounter,columnCounter,k,l);
					}
				}
			}
			
		}
	}
}


/* Return true if puzzle g is correct. */
int sudoku_is_correct(Grid_T g)
{
	int rowCounter,columnCounter,k,l,value;
	for(rowCounter = 0; rowCounter < MAXSIZE; rowCounter++)		/* for each row */
	{
		for(columnCounter = 0; columnCounter < MAXSIZE; columnCounter++)		/* for each column */
		{
			value = grid_read_value(g,rowCounter,columnCounter);
			if(value == 0)		/* if we have a 0 then sudoku is not correct so return 0*/
			{
				return 0;
			}
			for(k = 0; k < MAXSIZE; k++)
			{
				if(grid_read_value(g,rowCounter,k) == 0 || grid_read_value(g,k,columnCounter) == 0)		/* check if there is a 0 at the row or the column */
				{
					return 0;
				}
				if(k != columnCounter && value == grid_read_value(g,rowCounter,k))		/* check elements at row */
				{
					return 0;
				}
				if(k != rowCounter && value == grid_read_value(g,k,columnCounter))		/* check elements at column */
				{
					return 0;
				}
			}
			
			/* compute each substring and check it */
			for(k = (rowCounter/SUBGRIDSIZE) * SUBGRIDSIZE; k < (rowCounter/SUBGRIDSIZE) * SUBGRIDSIZE + SUBGRIDSIZE; k++)
			{
				for(l = (columnCounter/SUBGRIDSIZE) * SUBGRIDSIZE; l < (columnCounter/SUBGRIDSIZE) * SUBGRIDSIZE + SUBGRIDSIZE; l++)
				{
					if(k != rowCounter && l != columnCounter && value == grid_read_value(g,k,l))
					{
						return 0;
					}
				}
			}
			
		}
	}
	
	return 1;			/* sudoku is correct */
}


/* Sovle puzzle g and return the solved puzzle; if the puzzle has
   multiple solutions, return one of the possible solutions. */
Grid_T sudoku_solve(Grid_T g)
{
	int row, col, value, choice;
    Grid_T tmp, solvedSudoku;
    while ((value = sudoku_try_next(g, &row, &col))){
        choice = sudoku_update_choice(&g, row, col, value);		/* how much choices left */
        if (choice == 1){		/* if just 1, then apply the choice and eliminate all others from the same row-col */
            sudoku_apply_choice(&g, row, col, value);
            sudoku_eliminate_choice(&g, row, col, value);
        }
        else {	/* otherwise the sudoku solution is not unique, and we use a helper temp grid  to solve it */
            grid_clear_unique(&g);
            tmp = g;
            sudoku_apply_choice(&tmp, row, col, value);
            solvedSudoku = sudoku_solve(tmp);		/* recursion */
            if (sudoku_is_correct(solvedSudoku))	/* if we solved the sudoku, return it */
                return solvedSudoku;
        }
    }
    return g;		/* we did not solve it correct */
}


/* Returns true if solution g, as returned by sudoku_solve, has a
   unique choice for each step (no backtracking required). (Note, g
   must have been computed with the use of sudoku_solve.) */
int sudoku_solution_is_unique(Grid_T g)
{
	return grid_read_unique(g);
}

/* Main function used for getting input and printing output. For testing purposes. */
int main(int argc, char **argv){
  Grid_T init, final;
  FILE* fp = NULL;
  
  switch (argc) {
	case 2:         /* solve a sudoku (sudoku) */
		if(!(fp = fopen(argv[1],"r"))){
			printf("Cannot open input file\n");
			exit(1);
		}
		init = sudoku_read(fp);		/* init = input given from user */
		sudoku_print(stderr, init);
		sudoku_init_choices(&init);
		final = sudoku_solve(init);		/* final = solved sudoku */
		if (sudoku_is_correct(final)) {
			if (sudoku_solution_is_unique(final))
			  fprintf(stderr, "Puzzle has a unique solution:\n");
			else
			  fprintf(stderr, "Puzzle has a solution:\n");
			}
		else {
			fprintf(stderr, "Puzzle does not have solution:\n");
		}
		sudoku_print(stdout, final);		/* print the solved sudoku */
		sudoku_print_errors(final);		/* if it is not correct then print its errors */
		break;
    case 3:         /* check if sudoku is correct (sudoku -c) */
		if (argv[1][0] == '-' && argv[1][1] == 'c' && argv[1][2] == '\0') {
			if(!(fp = fopen(argv[2],"r"))){
				printf("Cannot open input file\n");
				exit(1);
			}
			init = sudoku_read(fp);		/* init = input given from user */
			sudoku_print(stderr, init);
			if (sudoku_is_correct(init)){
				fprintf(stderr, "Puzzle solution is correct.\n");
			}
			else{
				fprintf(stderr, "Puzzle solution is not correct.\n");
			}
			sudoku_print_errors(init);			/* if it is not correct then print its errors */
		} else{
			printf("First argument is wrong, -c is used for checking if a soduku solution is correct\n");
		}
		break;
	default:
		fprintf(stderr,"ERROR: Wrong number of arguments");
  }
   
  return 0;
}

