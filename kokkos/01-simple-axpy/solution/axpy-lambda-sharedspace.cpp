#include <Kokkos_Core.hpp>
#include <iostream>

template <typename T>
void axpy(const T *x, T *y, const T a, size_t N) 
{
  Kokkos::parallel_for(N,
     KOKKOS_LAMBDA (const size_t i) {
          y[i] += a * x[i];
     });
}

template <typename T>
void init(T x, T y, size_t N) 
{
  Kokkos::parallel_for(N,
    KOKKOS_LAMBDA (const size_t i) {
      x[i] = (i + 1) * 2.4;
      y[i] = (i + 1) * -1.2;
    });
}

int main(int argc, char** argv)
{
  Kokkos::initialize(argc, argv);

  using fp_type = double;

  const fp_type a = 0.5;
  constexpr size_t N = 100;

  fp_type *x = (fp_type *) Kokkos::kokkos_malloc<Kokkos::SharedSpace>(N * sizeof(fp_type));
  fp_type *y = (fp_type *) Kokkos::kokkos_malloc<Kokkos::SharedSpace>(N * sizeof(fp_type));

  init(x, y, N);
  Kokkos::fence();
  std::cout << "First and last elements before axpy: " << std::endl
            << "x: " << x[0] << "," << x[N-1] << std::endl
            << "y: " << y[0] << "," << y[N-1] << std::endl;

  axpy(x, y, a, N);
  Kokkos::fence();

  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << y[0] << "," << y[N-1] << std::endl;  

  Kokkos::finalize();
}
