// SPDX-FileCopyrightText: 2024 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char* argv[]) {

  // Initialize Kokkos
  Kokkos::initialize(argc, argv);

  {
    int n = 5;
    int m = 6;

    Kokkos::View<int*> a("a", n); // 1D array with runtime dimension 

    int rank = a.rank();
    std::cout << "Rank of a: " << rank << " dimensions: "; 
    for (int i=0; i < rank; i++)
      std::cout << a.extent(i) << " ";
    std::cout << std::endl;

    std::cout << "Execution space of a:    " << 
         decltype(a)::execution_space::name() << std::endl;
    std::cout << "Memory space of a:       " << 
         decltype(a)::memory_space::name() << std::endl;
  }
  Kokkos::finalize();
  return 0;
}
