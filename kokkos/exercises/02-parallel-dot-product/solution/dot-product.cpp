// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <Kokkos_Core.hpp>
#include <iostream>
#include <cmath>

int main(int argc, char** argv)
{
  Kokkos::initialize(argc, argv);

  constexpr size_t N = 100;

  double *x = (double *) malloc(N * sizeof(double));
  double *y = (double *) malloc(N * sizeof(double));

  // Initialize x and y
  Kokkos::parallel_for(N,
    KOKKOS_LAMBDA (const size_t i) {
    x[i] = cos(i * 2*M_PI / (N-1) );
    y[i] = sin(i * 2*M_PI / (N-1) );
  });
 
  std::cout << "First and last elements before dot product: " << std::endl
            << "x: " << x[0] << "," << x[N-1] << std::endl
            << "y: " << y[0] << "," << y[N-1] << std::endl;

  // Perform dot product
  double result = 0.0;
  Kokkos::parallel_reduce(N,
    KOKKOS_LAMBDA (const size_t i, double &local_result) {
    local_result += x[i] * y[i];
  }, result);

  // Check results
  std::cout << "Result (should be 0): " << result << std::endl;

  Kokkos::finalize();
}
