#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int *send = (int *)malloc(size * sizeof(int));
    int *recv = (int *)malloc(size * sizeof(int));

    for (int i = 0; i < size; i++) {
        send[i] = rank * size + i;
    }

    MPI_Alltoall(send, 1, MPI_INT, recv, 1, MPI_INT, MPI_COMM_WORLD);

    printf("Process %d received values: ", rank);
    for (int i = 0; i < size; i++) {
        printf("%d ", recv[i]);
    }
    printf("\n");

    free(send);
    free(recv);
    MPI_Finalize();
    return 0;
}