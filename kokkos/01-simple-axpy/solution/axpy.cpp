#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char** argv)
{
  Kokkos::initialize(argc, argv);

  const double a = 0.5;
  constexpr size_t N = 100;

  double *x = (double *) malloc(N * sizeof(double));
  double *y = (double *) malloc(N * sizeof(double));

  // Initialize x and y
  Kokkos::parallel_for(N,
    KOKKOS_LAMBDA (const size_t i) {
      x[i] = (i + 1) * 2.4;
      y[i] = (i + 1) * -1.2;
    });

  std::cout << "First and last elements before axpy: " << std::endl 
            << "x: " << x[0] << "," << x[N-1] << std::endl
            << "y: " << y[0] << "," << y[N-1] << std::endl;  

  // Apply axpy operation
  Kokkos::parallel_for(N,
     KOKKOS_LAMBDA (const size_t i) {
          y[i] += a * x[i];
     });

  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << y[0] << "," << y[N-1] << std::endl;  

  Kokkos::finalize();
}
