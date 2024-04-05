#include <Kokkos_Core.hpp>
#include <cstdio>

int main(int argc, char* argv[]) {
  Kokkos::initialize(argc, argv);
  {
  // Make View of length n > 1000 and add values
    const int n = 1000;
    Kokkos::View<int*> view("View", n);
    Kokkos::parallel_for("InitView", n, KOKKOS_LAMBDA(const int i) {
        view1(i) = i * i;
    });

  // create two additional views of same size and datatype
    Kokkos::View<int*> view_1("View1", n);
    Kokkos::View<int*> view_2("View2", n);
  // deep_copy
    Kokkos::Timer timer;
    Kokkos::deep_copy(view_2, view_1);
    double time_1 = timer.seconds();
  // user copy
    timer.reset()
    Kokkos::parallel_for("CopyView", n, KOKKOS_LAMBDA(const int i) {
        view3(i) = view1(i);
    });

    double time_2 = timer.seconds();
  // Output times 
    printf("Time for deep_copy: %f\n", time_1);
    printf("Time for parallel_for: %f\n", time_2);

  }
  Kokkos::finalize();
}
