#include <Kokkos_Core.hpp>
#include <iostream>

// With CUDA (compiler) functor needs to be defined in different scope than parallel dispatch
template <typename T1, typename T2>
struct AxpyFunctor {
    T1 _x, _y;
    const T2 _a;
    AxpyFunctor(T1 x, T1 y, T2 a) :
      _x(x), _y(y), _a(a) {};
    KOKKOS_INLINE_FUNCTION void operator()(const int i) const {
      _y[i] += _a * _x[i];
    }
};

template <typename T1, typename T2>
void axpy(T1 x, T1 y, T2 a, size_t N) 
{
  Kokkos::parallel_for(N, AxpyFunctor(x, y, a));
}

template <typename T>
struct InitFunctor {
    T _x, _y;
    InitFunctor(T x, T y) : _x(x), _y(y) {};
    KOKKOS_INLINE_FUNCTION void operator()(const size_t i) const {
      _x[i] = (i + 1) * 2.4;
      _y[i] = (i + 1) * -1.2;
    }
};

template <typename T>
void init(T x, T y, size_t N) 
{
  Kokkos::parallel_for(N, InitFunctor(x, y));
}

int main(int argc, char** argv)
{
  Kokkos::initialize(argc, argv);

  using fp_type = double;

  const fp_type a = 0.5;
  constexpr size_t N = 100;

  fp_type *x = (fp_type *) malloc(N * sizeof(fp_type));
  fp_type *y = (fp_type *) malloc(N * sizeof(fp_type));

  init(x, y, N);
  std::cout << "First and last elements before axpy: " << std::endl
            << "x: " << x[0] << "," << x[N-1] << std::endl
            << "y: " << y[0] << "," << y[N-1] << std::endl;


  axpy(x, y, a, N);

  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << y[0] << "," << y[N-1] << std::endl;  

  Kokkos::finalize();
}
