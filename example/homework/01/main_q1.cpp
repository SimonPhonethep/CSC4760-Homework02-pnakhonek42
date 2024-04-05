#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int process_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    int token;
    if (process_rank != 0) {
        MPI_Recv(&token, 1, MPI_INT, process_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d has received the token %d from process %d\n", process_rank, token, process_rank - 1);
    } else {
        token = 0;
    }
    token++;
    MPI_Send(&token, 1, MPI_INT, (process_rank + 1) % size, 0, MPI_COMM_WORLD);

    if (process_rank == 0) {
        MPI_Recv(&token, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d has received the token %d from process %d\n", process_rank, token, size - 1);
    }

    MPI_Finalize();
    return 0;
}