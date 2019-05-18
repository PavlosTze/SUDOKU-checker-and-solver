sudoku: sudoku.o grid.o
	gcc -Wall -ansi -pedantic sudoku.o grid.o -o sudoku

sudoku.o: sudoku.c
	gcc -Wall -ansi -pedantic -c sudoku.c

grid.o: grid.c
	gcc -Wall -ansi -pedantic -c grid.c

clean:
	rm sudoku
	rm sudoku.o grid.o
