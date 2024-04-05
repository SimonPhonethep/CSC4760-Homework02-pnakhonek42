#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);

    int process_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    int P = 1, Q = 1;


    MPI_Comm split_comm1;
    int color1 = process_rank/ Q;
    MPI_Comm_split(MPI_COMM_WORLD, color1, process_rank, &split_comm1);


    MPI_Comm split_comm2;
    int color2 = process_rank % Q;
    MPI_Comm_split(MPI_COMM_WORLD, color2, process_rank, &split_comm2);

    MPI_Comm_free(&split_comm1);
    MPI_Comm_free(&split_comm2);

    MPI_Finalize();
    return 0;
}