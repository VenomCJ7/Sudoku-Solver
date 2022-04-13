# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <pthread.h>
# include <stdbool.h>
# include <unistd.h>

void read_grid_from_file(int size, char *ip_file, int grid[36][36]){
	FILE *fp;
	int i, j;
	fp = fopen(ip_file, "r");
	for (i=0; i<size; i++) 
		for (j=0; j<size; j++) {
			fscanf(fp, "%d", &grid[i][j]);
	    }
} 

void print_grid(int size, int grid[36][36]){
	int i, j;
	/* The segment below prints the grid in a standard format. Do not change */
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++)
			printf("%d\t", grid[i][j]);
		printf("\n");
	}
}

int sqr(int size){
	if(size==1) return 1;
	else if(size==4) return 2;
	else if(size==9) return 3;
	else if(size==16) return 4;
	else if(size==25) return 5;
	else if(size==36) return 6;
	else return 0;
}

bool findEmptyPlace(int size, int grid[36][36], int *row, int *col){ //get empty location and update row and column
    for (*row = 0; *row < size; (*row)++)
        for (*col = 0; *col < size; (*col)++)
            if (grid[*row][*col] == 0) return true;
    return false;
}

typedef struct{
	int row;
	int col;
	int size;
	int num;
	int (*grid)[36];
}  cells;

void * inrows(void * cell) {
    cells * data = (cells *) cell;
	int col = data->col;
	int row = data->row;
	int num = data->num;
	int size = data->size;
    for (int col = 0; col < size; col++){
		int val = data->grid[row][col];
		if (val == num) return (void *) 0;
	}
    return (void *) 1;
}

void * incols(void * cell) {
    cells * data = (cells *) cell;
	int col = data->col;
	int row = data->row;
	int num = data->num;
	int size = data->size;
    for (int row = 0; row < size; row++){
		int val = data->grid[row][col];
		if (val == num) return (void *) 0;
	}
    return (void *) 1;
}

void * insub(void * cell) {
    cells * data = (cells *) cell;
	int col = data->col;
	int row = data->row;
	int num = data->num;
	int size = data->size;
	int s = sqr(size);
	int startRow = row - row % s; 
	int startCol = col - col % s;
	for (int r = 0; r < s; r++)
		for (int c = 0; c < s; c++){
            int val = data->grid[r + startRow][c + startCol];
            if (val == num) return (void *) 0;
        }	
	return (void *) 1;
}


bool validate(int size, int grid[36][36], int row, int col, int num){

    cells * cell = (cells *) malloc(sizeof(cells));
	cell->size=size;
	cell->row=row;
	cell->col=col;
	cell->grid=grid;
	cell->num=num;

	pthread_t Row, Col, Sub;
	pthread_create(&Row, NULL, inrows, (void *) cell);
    pthread_create(&Col, NULL, incols, (void *) cell);
    pthread_create(&Sub, NULL, insub, (void *) cell);
    void * inRow;
    void * inCol;
    void * inSub;
	pthread_join(Row, &inRow);//closing pthread Row
    pthread_join(Col, &inCol);
    pthread_join(Sub, &inSub);

	if((intptr_t) inRow==1 && (intptr_t) inCol==1 && (intptr_t) inSub==1) return true;
    return false;
}

bool sudokuSolver(int size, int grid[36][36]){
    int row, col;
    if (!findEmptyPlace(size, grid, &row, &col)) return true; //when all places are filled, sudoku has been solved
    for (int num = 1; num <= size; num++){
        //thread here for every valid number from 1 to size	
        if(validate(size, grid, row, col, num)){
            grid[row][col] = num;
            if(sudokuSolver(size, grid)) return true;
            grid[row][col] = 0; //turn to unassigned space when conditions are not satisfied
        }
    }
    return false;	
}

int main(int argc, char *argv[]) {
	int grid[36][36], size, i, j;
	
	if (argc != 3) {
		printf("Usage: ./sudoku.out grid_size inputfile");
		exit(-1);
	}
	
	size = atoi(argv[1]);
	read_grid_from_file(size, argv[2], grid);
	
	/* Do your thing here */
	if(sudokuSolver(size, grid)) print_grid(size, grid);

	else printf("No solution exists\n");
    return 0;
}