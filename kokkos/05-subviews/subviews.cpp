#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char* argv[]) {

  // Initialize Kokkos
  Kokkos::initialize(argc, argv);

  {
    int n = 20;
    int m = 20;

    Kokkos::View<int**> a("a", n, m); 

    // Subviews of boundaries

    // Initialize boundaries in parallel

    // For copying to host, we need contiguous buffers in device
    

    // Create mirror views

    // Copy to host

    Kokkos::fence();

  }
  Kokkos::finalize();
  return 0;
}
