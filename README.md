# Parallel All-Pairs Shortest Path (APSP) algorithm
A cost-optimal parallel algorithm based on Floyd-Warshall Algorithm in order to resolve the APSP problem

The sequential version of the Floyd’s algorithm takes in input the adjacency matrix of size 𝑛 × 𝑛, whose entries (𝑖, 𝑗) represent the positive weights that refer to the distance between vertex 𝑖 and vertex 𝑗, for 𝑖, 𝑗 = 1, ... , 𝑛. The same matrix in output will contain the updated weights representing the shortest paths for each pair of nodes. The development of the parallel version was done according to Foster’s PCAM methodology.

## Prerequisites
