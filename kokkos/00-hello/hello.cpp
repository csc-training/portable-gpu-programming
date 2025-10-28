#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char* argv[]) {

  Kokkos::initialize(argc, argv);

  Kokkos::finalize();

  return 0;
}
