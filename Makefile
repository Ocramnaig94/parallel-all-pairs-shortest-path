CC=mpicc
SRC=MyMPI.c
SRC1=allshortestpair.c
SRC2=gen_adj_matrix.c
EXEC1=allshortestpair
EXEC2=gen_adj_matrix
CFLAGS= -O3

all: $(EXEC1) $(EXEC2) 

$(EXEC1): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(SRC1)
	
$(EXEC2): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(SRC2)
	
clean:
	rm -f $(EXEC1) $(EXEC2)  *.o *~