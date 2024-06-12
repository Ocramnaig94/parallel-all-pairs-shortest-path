#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#include "MyMPI.h"

int main(int argc, char **argv){

	int rank, **submatrix = NULL;
	GRID grid;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &(grid.p));

	if (argc != 2) {
		if (!rank){
			fprintf(stderr, "Usage: %s <matrix file>\n", argv[0]);
			MPI_Barrier(MPI_COMM_WORLD);
 			MPI_Abort(MPI_COMM_WORLD, INPUT_ERROR);
 		}
 		MPI_Barrier(MPI_COMM_WORLD);
   	}

	// Check if perfect square
	int i = 1;
	while ((i * i) < grid.p) i++;
	if ((i * i) != grid.p){
		if (!rank){
			fprintf(stderr, "Number of processors must be a perfect square\n");
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Abort(MPI_COMM_WORLD, PROC_ERROR);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	else
		grid.q = (int) sqrt(grid.p);
	
	// Setup communicators
	initial_setup(&grid);

	// Obtain the respective submatrix for each process
	submatrix = manage_matrix(&grid, argv[1]);
	
 	//Print only small input matrices
 	if (grid.dim <= 32)  
 		print_matrix(&grid, &submatrix);

 	floyd(&grid, &submatrix);
 	
 	//Print only small output matrices
 	if (grid.dim <= 32)  
 		print_matrix(&grid, &submatrix);

	MPI_Comm_free(&grid.row_com);
	MPI_Comm_free(&grid.col_com);
	MPI_Comm_free(&grid.com);
	MPI_Finalize();

	return 0;
}