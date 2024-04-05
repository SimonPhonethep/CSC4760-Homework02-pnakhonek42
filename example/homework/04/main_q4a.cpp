#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int send_sum = 5;
    int recv;

    MPI_Bcast(&send, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Reduce(&send, &recv, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Bcast(&recv, 1, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Process %d received value %d\n", rank, recv);

    MPI_Finalize();
    return 0;
}