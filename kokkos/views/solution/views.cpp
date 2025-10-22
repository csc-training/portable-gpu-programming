#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char* argv[]) {

  // Initialize Kokkos
  Kokkos::initialize(argc, argv);

  {
    int n = 5;
    int m = 6;

    Kokkos::View<int*> a("a", n); // 1D array with runtime dimension 
    Kokkos::View<int*, Kokkos::HostSpace> h_a("h_a", n); // 1D array with runtime dimension 
    Kokkos::View<double*[6]> b("b", n); // 2D n x 6 array with compile time dimension
    Kokkos::View<double**> b2 ("b2", n, m);
    Kokkos::View<double**, Kokkos::HostSpace> h_b("h_b", n, m);
    Kokkos::View<double**, Kokkos::SharedSpace> s_b("s_b", n, m);
    Kokkos::View<double**, Kokkos::Device<Kokkos::Serial, Kokkos::SharedSpace> > s2_b("s2_b", n, m);

    std::cout << "Execution space of a:    " << 
         decltype(a)::execution_space::name() << std::endl;
    std::cout << "Memory space of a:       " << 
         decltype(a)::memory_space::name() << std::endl;
    std::cout << "Execution space of h_b:  " << 
         decltype(h_b)::execution_space::name() << std::endl;
    std::cout << "Memory space of h_b:     " << 
         decltype(h_b)::memory_space::name() << std::endl;
    std::cout << "Execution space of s_b:  " << 
         decltype(s_b)::execution_space::name() << std::endl;
    std::cout << "Memory space of s_b:     " << 
         decltype(s_b)::memory_space::name() << std::endl;
    std::cout << "Execution space of s2_b: " << 
         decltype(s2_b)::execution_space::name() << std::endl;
    std::cout << "Memory space of s2_b:    " << 
         decltype(s2_b)::memory_space::name() << std::endl;
  }
  Kokkos::finalize();
  return 0;
}
