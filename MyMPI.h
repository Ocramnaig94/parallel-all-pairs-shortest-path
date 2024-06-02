typedef struct {
	MPI_Comm row_com;
	MPI_Comm col_com;	
	MPI_Comm com;		
	int rank;   		
	int row_rank;		
	int col_rank;		
	int p;			    
	int q;				
	int coords[2];		
	int dim;			
	int sub_rows;		
	int sub_cols;		
} GRID;


#define BLOCK_LOW(rank,p,n)  (rank * n / p)
#define BLOCK_HIGH(rank,p,n) (BLOCK_LOW((rank + 1),p,n) - 1)
#define BLOCK_SIZE(rank,p,n) (BLOCK_HIGH(rank,p,n) - BLOCK_LOW(rank,p,n) + 1)
#define BLOCK_OWNER(idx, p, n) ( (p * (idx + 1)-1) / n)
#define MIN(a,b) ( (a < b) ? a : b )
#define INF INT_MAX/2

#define INPUT_ERROR  -1
#define PROC_ERROR   -2
#define OPEN_ERROR   -3
#define FILE_ERROR   -4
#define CALLOC_ERROR -5

void initial_setup(GRID* grid);
int **manage_matrix(GRID *grid, char *filename);
int **alloc_matrix(long rows, long cols);
void print_buffer(long size, int *buffer);
void print_matrix(GRID *grid, int ***matrix);
void floyd(GRID* grid, int ***submatrix);
