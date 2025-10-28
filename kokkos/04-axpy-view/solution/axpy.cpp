#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char** argv)
{
  Kokkos::initialize(argc, argv);

  const double a = 0.5;
  constexpr size_t N = 100;

  Kokkos::View<double *> x("x", N);
  Kokkos::View<double *> y("x", N);

  Kokkos::View<double *, Kokkos::HostSpace> h_x("x", N);
  Kokkos::View<double *, Kokkos::HostSpace> h_y("y", N);

  // Initialize x and y
  Kokkos::parallel_for(N,
    KOKKOS_LAMBDA (const size_t i) {
      x[i] = (i + 1) * 2.4;
      y[i] = (i + 1) * -1.2;
    });

  // copy to host for printing
  Kokkos::deep_copy(h_x, x);
  Kokkos::deep_copy(h_y, y);

  std::cout << "First and last elements before axpy: " << std::endl 
            << "x: " << x[0] << "," << x[N-1] << std::endl
            << "y: " << y[0] << "," << y[N-1] << std::endl;  

  // Apply axpy operation
  Kokkos::parallel_for(N,
     KOKKOS_LAMBDA (const size_t i) {
          y[i] += a * x[i];
     });

  // copy to host for printing
  Kokkos::deep_copy(h_y, y);

  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << y[0] << "," << y[N-1] << std::endl;  

  Kokkos::finalize();
}
