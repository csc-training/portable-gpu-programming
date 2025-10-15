#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char* argv[]) {

  Kokkos::initialize(argc, argv);

  std::cout << "Execution Space: " <<
    Kokkos::DefaultExecutionSpace::name() << std::endl;
  std::cout << "Memory Space: " <<
    Kokkos::DefaultExecutionSpace::memory_space::name() << std::endl;

  Kokkos::finalize();

  return 0;
}
