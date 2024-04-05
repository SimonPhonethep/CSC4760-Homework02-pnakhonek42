#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> 

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int send = 5;
    int recv;

    if (rank == 0) {
        recv = send; 
    }

    MPI_Bcast(&recv, 1, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Process %d received value: %d\n", rank, recv);

    MPI_Finalize();
    return 0;
}