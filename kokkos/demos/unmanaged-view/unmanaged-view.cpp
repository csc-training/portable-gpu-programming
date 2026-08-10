// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char* argv[]) {

  // Initialize Kokkos
  Kokkos::initialize(argc, argv);

  {
    constexpr size_t N = 100;

    // create a raw host pointer with malloc
    double *x_ptr = (double *) malloc(N * sizeof(double));

    // Initialize x 
    for (size_t i = 0; i < N; i++)
    {
      x_ptr[i] = (i + 1) * 2.4;
    }     

    // print out through pointer
    std::cout << "First and last elements via ptr: " << std::endl
              << x_ptr[0] << "," << x_ptr[N-1] << std::endl;

    // Create unmanaged Kokkos View on HostSpace
    Kokkos::View<double*, Kokkos::HostSpace, Kokkos::MemoryUnmanaged> x(x_ptr, N);

    // print out through View
    std::cout << "First and last elements via View: " << std::endl
              << x[0] << "," << x[N-1] << std::endl;

  }
  Kokkos::finalize();
  return 0;
}
