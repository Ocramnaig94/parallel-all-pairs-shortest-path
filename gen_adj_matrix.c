#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <limits.h>

#define INF 		   INT_MAX/2 
#define DEF_MAX_WEIGHT    9 
#define GENERATE(i,j,r,w) (i==j) ? 0: ( ( (r = 1 + rand() % (w+1) ) == w+1) ? INF : r )

#define INPUT_ERROR  -1
#define SIZE_ERROR   -2
#define MALLOC_ERROR -3

void print_matrix(int rows, int cols, int ***matrix);

int main(int argc, char **argv){

	if (argc < 3 || argc > 4) {
    	printf ("Usage: %s <matrix size> <file> optional: <max weight>\n", argv[0]);
	  	fflush(stdout);
        exit(INPUT_ERROR);
    }

    srand(time(0));
   
    FILE *fout;
    int **A, *Astorage, random;
    long max_weight, size;
    
    size = strtol(argv[1], NULL, 10); 
    
    if (size <= 2){
   		printf ("<matrix size> must be a number at least equal to 3\n");
	  	fflush(stdout);
      	exit(SIZE_ERROR);
    } 
    if (size == LONG_MAX){
   		printf ("Matrix size is too big\n");
	  	fflush(stdout);
      	exit(SIZE_ERROR);
    }
    
    if (argv[3])
    	max_weight = strtol(argv[3], NULL, 10);
    else 
    	max_weight = DEF_MAX_WEIGHT;
    	
    if (max_weight < 1){
   		printf ("<max weight> must be a number at least equal to 1\n");
	  	fflush(stdout);
      	exit(SIZE_ERROR);
    }
	if (max_weight >= INF){
   		printf ("Max weight is too big\n");
	  	fflush(stdout);
      	exit(SIZE_ERROR);
    }

    Astorage = (int *) malloc(size * size * sizeof(int));
    A = (int **) malloc(size * sizeof(int *));
   	if (!Astorage || !A){
   		printf ("Not enough memory\n");
	  	fflush(stdout);
      	exit(MALLOC_ERROR);
	}
	
	for (int i = 0; i < size; i++){
		A[i] = &Astorage[i*size];
		for (int j = 0; j < size; j++)
 			A[i][j] = GENERATE(i,j,random,max_weight);
	}
		
	fout = fopen(argv[2], "wb");
    fwrite(&size, sizeof(long), 1, fout);
    fwrite(Astorage, sizeof(int), size * size, fout);
   	fclose(fout);

 	// Print only small matrices
 	if (size <= 32)   
		print_matrix(size, size, &A);
	
	free(Astorage), Astorage = NULL;
	free(A), A = NULL;

    return 0;
}

void print_matrix(int rows, int cols, int ***matrix){

	for (int i = 0; i < rows; i++){
    	for (int j = 0; j < cols; j++){
    		if ((*matrix)[i][j] == INF)
    			printf("%4s   ", "INF"); 
    		else
        		printf("%4d   ", (*matrix)[i][j]); 
        }
    printf("\n"); 
    }
}