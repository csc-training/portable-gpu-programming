#include <Kokkos_Core.hpp>
#include <iostream>

template <typename T1, typename T2>
void axpy(T1 x, T1 y, T2 a, size_t N) 
{
  Kokkos::parallel_for(N,
     KOKKOS_LAMBDA (const size_t i) {
          y(i) += a * x(i);
     });
}

template <typename T>
void init(T x, T y, size_t N) 
{
  Kokkos::parallel_for(N,
    KOKKOS_LAMBDA (const size_t i) {
      x(i) = (i + 1) * 2.4;
      y(i) = (i + 1) * -1.2;
    });
}

int main(int argc, char** argv)
{
  Kokkos::initialize(argc, argv);
  {

  using fp_type = double;

  const fp_type a = 0.5;
  constexpr size_t N = 100;

  Kokkos::View<fp_type [N]> x("x");
  Kokkos::View<fp_type [N]> y("y");

  Kokkos::View<fp_type [N], Kokkos::HostSpace> h_x("host x");
  Kokkos::View<fp_type [N], Kokkos::HostSpace> h_y("host y");

  init(x, y, N);

  // Copy data to host for printing
  Kokkos::deep_copy(h_x, x);
  Kokkos::deep_copy(h_y, y);
  Kokkos::fence();

  std::cout << "First and last elements before axpy: " << std::endl 
            << "x: " << h_x[0] << "," << h_x[N-1] << std::endl
            << "y: " << h_y[0] << "," << h_y[N-1] << std::endl;  

  axpy(x, y, a, N);

  // Copy data to host for printing
  Kokkos::deep_copy(h_y, y);
  Kokkos::fence();

  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << h_y[0] << "," << h_y[N-1] << std::endl;  

  }
  Kokkos::finalize();
}
