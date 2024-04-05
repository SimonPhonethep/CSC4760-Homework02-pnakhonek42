#include <Kokkos_Core.hpp>
#include <mpi.h>
#include <cstdio>

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  Kokkos::initialize(argc, argv);
  {
  // Make View and init values
    int n = 10;
    int process_rank;
    Kokkos::View<double*> view("view", n);
  // Send View values with MPI functions
    if (process_rank == 0) {
      Kokkos::parallel_for(n, KOKKOS_LAMBDA(const int i) {
          view(i) = i;
      });
      Kokkos::fence();
      MPI_Send(view.data(), n, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);

    } else if (process_rank == 1) {
      MPI_Recv(view.data(), n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      Kokkos::parallel_for(n, KOKKOS_LAMBDA(const int i) {
          printf("view(%d) = %f\n", i, view(i));
      });
      Kokkos::fence();
    }
  // Output
  printf("\nhello world\n");
  }
  Kokkos::finalize();
  MPI_Finalize();
}
