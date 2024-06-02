#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mpi.h"
#include "MyMPI.h"

void initial_setup(GRID *grid){
	
	// Create a new communicator with cartesian topology
	int dim_sizes[2] = {0, 0}, wrap_around[2] = {0, 0}, reorder = 1;
    
    MPI_Dims_create(grid->p, 2, dim_sizes);
    MPI_Cart_create(MPI_COMM_WORLD, 2, dim_sizes, wrap_around, reorder, &(grid->com));
    
    MPI_Comm_rank(grid->com, &(grid->rank));
    MPI_Cart_coords(grid->com, grid->rank, 2, grid->coords);
    
    // Create a new communicator for each row of the grid
	int row_dims[2] = {0, 1};
    MPI_Cart_sub(grid->com, row_dims, &(grid->row_com));   
    MPI_Comm_rank(grid->row_com, &(grid->row_rank));
    
    // Create a new communicator for each column of the grid
	int col_dims[2] = {1, 0};
    MPI_Cart_sub(grid->com, col_dims, &(grid->col_com));  
    MPI_Comm_rank(grid->col_com, &(grid->col_rank));
	
}

int **manage_matrix(GRID *grid, char *filename){
	
	int **submatrix = NULL;
	FILE *filein;
	
	if (grid->rank == (grid->p)-1){

		filein = fopen(filename, "rb");
    	if (!filein){
			fprintf(stderr, "File: %s not found\n", filename); 
			MPI_Abort(MPI_COMM_WORLD, OPEN_ERROR);
		}
		
    	fread(&(grid->dim), sizeof(long), 1, filein);
   		
   		if (grid->q > grid->dim){
			fprintf(stderr, "Too many processors\n");
			fclose(filein);
			MPI_Abort(MPI_COMM_WORLD, PROC_ERROR);
		}
    }

	MPI_Bcast(&(grid->dim), 1, MPI_LONG, ((grid->p)-1), grid->com);

	// Set dimension for all submatrices
	grid->sub_rows = BLOCK_SIZE(grid->coords[0], grid->q, grid->dim);
    grid->sub_cols = BLOCK_SIZE(grid->coords[1], grid->q, grid->dim); 
    
    // Allocate local submatrices
    submatrix = alloc_matrix(grid->sub_rows, grid->sub_cols);

	if (grid->rank == (grid->p)-1){
	
		int coords[2];
		long sub_rows, sub_cols;
		MPI_Request req;
		fpos_t next;
		
		int *buffer = (int *) calloc(grid->sub_rows * grid->sub_cols, sizeof(int));
		if (!buffer){
			fprintf(stderr, "Not enough memory"); 
			MPI_Abort(MPI_COMM_WORLD, CALLOC_ERROR);
		}
		
		for (int rank = 0; rank < grid->p; rank ++){
			
			MPI_Cart_coords(grid->com, rank, 2, coords);
			
			// Obtain rows and column of the specific processor
			sub_rows = BLOCK_SIZE(coords[0], grid->q, grid->dim);
			sub_cols = BLOCK_SIZE(coords[1], grid->q, grid->dim);
			
			// Restore point to the respective submatrix
			if (coords[1])
				fsetpos(filein, &next);
	
 			for (long row = 0; row < sub_rows; row++){
 			
 				// Wait for previous send
 				if (rank && !row)
					MPI_Wait(&req, MPI_STATUS_IGNORE);
 					
 				fread(&buffer[row * sub_cols], sizeof(int), sub_cols, filein);
 				
 				// Save pointer to next submatrix	
 				if (!row && (coords[1] != (grid->q)-1) )
 					fgetpos(filein, &next);

				// Go to next row, if not last row
				if (row < sub_rows-1)
					fseek(filein, (grid->dim - sub_cols) * sizeof(int), SEEK_CUR);
 			
 			}

			if (rank < grid->rank)
				MPI_Isend(buffer, sub_rows*sub_cols, MPI_INT, rank, 0, grid->com, &req);
				
			// The last process
			else{
				for (long i = 0; i < grid->sub_rows; i++)
        			for (long j = 0; j < grid->sub_cols; j++)  				
        				submatrix[i][j] = buffer[i * grid->sub_cols + j];
			}
		}
		free(buffer), buffer = NULL;
		fclose(filein);
    }
    else
   		MPI_Recv(&submatrix[0][0], grid->sub_rows*grid->sub_cols, MPI_INT, (grid->p)-1, 0, grid->com, MPI_STATUS_IGNORE);

    return submatrix;
}

int **alloc_matrix(long rows, long cols){

    int *mat = (int *) calloc(rows * cols, sizeof(int));	
    int **matrix = (int **) calloc(rows, sizeof(int *));	
    if (!mat || !matrix){
		fprintf(stderr, "Not enough memory"); 
		MPI_Abort(MPI_COMM_WORLD, CALLOC_ERROR);
	}
	
    for (long i = 0; i < rows; i++)
        matrix[i] = &(mat[i*cols]);
        
    return matrix;
}

void print_buffer(long size, int *buffer){

	for (long i = 0; i < size; i++)
		if (buffer[i] == INF)
			fprintf(stderr, "%4s   ", "INF"); 
    	else
			fprintf(stderr, "%4d   ", buffer[i]); 
}

void print_matrix(GRID *grid, int ***submatrix){

	int rank;
	long sub_rows, sub_cols;
	
	if (grid->rank == (grid->p)-1 ){
	
		int *buffer = (int *) calloc(grid->sub_cols, sizeof(int));
		if (!buffer){
			fprintf(stderr, "Not enough memory"); 
			MPI_Abort(MPI_COMM_WORLD, CALLOC_ERROR);
		}
	
	 	fprintf(stderr, "%d x %d Matrix: \n", grid->dim, grid->dim); 	
	 		
		// For each row communicator
		for (int i = 0; i < grid->q; i++){
		
			sub_rows = BLOCK_SIZE(i, grid->q, grid->dim);

			// For each row of the submatrix
			for (long it = 0; it < sub_rows; it++){
							
				// For each row process within row rank 
				for (int j = 0; j < grid->q; j++){
				
					sub_cols = BLOCK_SIZE(j, grid->q, grid->dim);
				
					rank = i * grid->q + j;
					
					if (rank == grid->rank)	
						print_buffer(grid->sub_cols, (*submatrix)[it]);
					else{	
						MPI_Recv(buffer, sub_cols, MPI_INT, rank, 0, grid->com, MPI_STATUS_IGNORE);
						print_buffer(sub_cols, buffer);
					}
				}
				fprintf(stderr, "\n"); 	
			}
		}		
		free(buffer), buffer = NULL;
	}
	else{
		for (long i = 0; i < grid->sub_rows; i++)
			MPI_Send((*submatrix)[i], grid->sub_cols, MPI_INT, (grid->p)-1, 0, grid->com);
	}
}

void floyd(GRID* grid, int ***submatrix){

	int root;
	long local_col = 0, local_row = 0;
	int *col_vector = (int *) calloc(grid->sub_rows, sizeof(int));
	int *row_vector = (int *) calloc(grid->sub_cols, sizeof(int));
	if (!col_vector || !row_vector){
		fprintf(stderr, "Not enough memory"); 
		MPI_Abort(MPI_COMM_WORLD, CALLOC_ERROR);
	}
    MPI_Request r_row,r_col;
    
    // Benchmark
    MPI_Barrier(grid->com);
 	double time = - MPI_Wtime(); 
 	
	for (long it = 0; it < grid->dim; it++){
	
    	root = BLOCK_OWNER(it, grid->q, grid->dim);
    	
 		if (grid->row_rank == root){
 			// Wait for previous column sent
 		    if (local_col)
    			MPI_Wait(&r_col, MPI_STATUS_IGNORE);
 			for (long i = 0; i < grid->sub_rows; i++)
    			col_vector[i] = (*submatrix)[i][local_col];

    		MPI_Ibcast(&col_vector[0], grid->sub_rows , MPI_INT, root, grid->row_com, &r_col);
    		local_col++;
    	}
    	
    	 if (grid->col_rank == root){
    		 // Wait for previous row sent
    	    if (local_row)
    			MPI_Wait(&r_row, MPI_STATUS_IGNORE);
 			for (long j = 0; j < grid->sub_cols; j++)
    			row_vector[j] = (*submatrix)[local_row][j];

    		MPI_Ibcast(&row_vector[0], grid->sub_cols , MPI_INT, root, grid->col_com, &r_row);
    		local_row++;
    	}
    	
    	if (grid->row_rank != root){
 			MPI_Ibcast(&col_vector[0], grid->sub_rows , MPI_INT, root, grid->row_com, &r_col);
 			MPI_Wait(&r_col, MPI_STATUS_IGNORE);
 		}
 		
    	if (grid->col_rank != root){
    	 	MPI_Ibcast(&row_vector[0], grid->sub_cols , MPI_INT, root, grid->col_com, &r_row);
    	 	MPI_Wait(&r_row, MPI_STATUS_IGNORE);
    	}

 		for (long i = 0; i < grid->sub_rows; i++)
 			for (long j = 0; j < grid->sub_cols; j++)
 				(*submatrix)[i][j] = MIN( (*submatrix)[i][j], (row_vector[j] + col_vector[i]) );
	}
	
	MPI_Barrier(grid->com);
	time += MPI_Wtime(); 
	
	if (grid->rank == grid->p-1)
		fprintf(stderr, "Execution time: %f\n", time);
			
	free(col_vector), col_vector = NULL;
	free(row_vector), row_vector = NULL;			
}