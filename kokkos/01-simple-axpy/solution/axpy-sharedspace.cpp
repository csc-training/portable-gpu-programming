#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char** argv)
{
  Kokkos::initialize(argc, argv);

  const double a = 0.5;
  constexpr size_t N = 100;

  // kokkos_malloc is drop-in replacement for standard malloc, which can allocate
  // memory either from host, device, or use shared (= unified/managed) memory.
  // Here we allocate memory which is shared between host and device 
  // (hardware takes care of moving data between physical CPU and GPU memories)
  // Generally, using kokkos_malloc is not recommended, but one should rather use
  // Kokkos Views that will be discussed shortly
  double *x = (double *) Kokkos::kokkos_malloc<Kokkos::SharedSpace>(N * sizeof(double));
  double *y = (double *) Kokkos::kokkos_malloc<Kokkos::SharedSpace>(N * sizeof(double));

  // Initialize x and y
  Kokkos::parallel_for(N,
    KOKKOS_LAMBDA (const size_t i) {
      x[i] = (i + 1) * 2.4;
      y[i] = (i + 1) * -1.2;
    });
  // Due to asynchronous execution, kernel could still be running when program gets to 
  // printing, so we synchronize the device
  Kokkos::fence();

  std::cout << "First and last elements before axpy: " << std::endl
            << "x: " << x[0] << "," << x[N-1] << std::endl
            << "y: " << y[0] << "," << y[N-1] << std::endl;

  // Apply axpy operation
  Kokkos::parallel_for(N,
     KOKKOS_LAMBDA (const size_t i) {
          y[i] += a * x[i];
     });
  Kokkos::fence();

  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << y[0] << "," << y[N-1] << std::endl;  

  Kokkos::finalize();
}
