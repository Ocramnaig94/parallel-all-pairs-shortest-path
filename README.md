# Parallel All-Pairs Shortest Path (APSP) algorithm
A cost-optimal parallel algorithm based on Floyd-Warshall Algorithm in order to resolve the APSP problem

The sequential version of the Floyd’s algorithm takes in input the adjacency matrix of size 𝑛 × 𝑛, whose entries (𝑖, 𝑗) represent the positive weights that refer to the distance between vertex 𝑖 and vertex 𝑗, for 𝑖, 𝑗 = 1, ... , 𝑛. The same matrix in output will contain the updated weights representing the shortest paths for each pair of nodes. The development of the parallel version was done according to Foster’s PCAM methodology.

## Prerequisites

Install [MPICH](https://www.mpich.org/), a high performance implementation of the Message Passing Interface (MPI) standard.

Build the executables by using `Makefile`

```
make all
```

## How to Run
* Generate first the input square matrix through `gen_adj_matrix`, by providing the number or rows/columns and the name of the binary output file `.bin`. Optionally, a user-defined maximum weight can be set instead of the default one
```
./gen_adj_matrix <matrix size> <file> optional: <max weight>
```
* Alternatively, as the input square matrix a predefined one inside the homonymous folder `matrices` can be used.

* Run the APSP algorithm by providing the number of processors, which must be a [perfect square](https://en.wikipedia.org/wiki/Square_number), and the binary file containing the adjacency matrix
```
mpiexec -n <number of processors> ./allshortestpair <file>
```

## Benchmark
The benchmark was performed on a virtual machine on a single CPU machine with 4 physical cores, and 8 logical ones due to hyper-threading.

| Matrix size | Time on 1 processor | Time on 4 processors | Speedup | Efficiency |
| ----- | :---: | :---: | :---: | :---:  |
| 1000 × 1000  | 1,7 seconds | 0,55 seconds | 3 | 0,75 |
| 3000 × 3000  | 38 seconds | 12 seconds | 3,16 | 0,79 |
| 5000 × 5000  | 174 seconds | 55 seconds | 3,16 | 0,79 |
