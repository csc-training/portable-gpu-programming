#include <Kokkos_Core.hpp>
#include <iostream>

// With CUDA (compiler) functor needs to be defined in different scope than parallel dispatch
// As we need to make the functor now generic enough to be used with different types of Views,
// a bit more advanced C++ template techniques are used here.

// First, Kokkos View might be templated both with the datatype (type and dimensionality)
// as well as various "properties" (memory layout, non-default memory space, ...).
// Thus, we define the template with the "datatype" DT as well as with a **template pack** DP
// for the properties. Finally, View has C++ trait ViewTrait that we can use for deducing
// the type for the scalar a
template <typename DT, typename... DP>
class AxpyFunctor {
    const Kokkos::View<DT, DP...> _x;
    Kokkos::View<DT, DP...> _y;
    typename Kokkos::ViewTraits<DT, DP...>::const_value_type _a;
  public:
    AxpyFunctor(Kokkos::View<DT, DP...> x, Kokkos::View<DT, DP...> y, 
                typename Kokkos::ViewTraits<DT, DP...>::const_value_type a) :
                _x(x), _y(y), _a(a) {};

    KOKKOS_INLINE_FUNCTION void operator()(const int i) const {
      _y(i) += _a * _x(i);
    }
};

// The same template programming techniques are used for defining a function
// that takes Views as arguments
template <typename DT, typename... DP>
void axpy(Kokkos::View<DT, DP...> x, Kokkos::View<DT, DP...> y, 
          typename Kokkos::ViewTraits<DT, DP...>::const_value_type a) 
{
  int N = x.extent(0);
  Kokkos::parallel_for(N, AxpyFunctor(x, y, a));
}

int main(int argc, char** argv)
{
  Kokkos::initialize(argc, argv);
  {

  const double a = 0.5;
  constexpr size_t N = 100;

  Kokkos::View<double *> x("x", N);
  Kokkos::View<double *> y("y", N);

  Kokkos::View<double *, Kokkos::HostSpace> h_x("host x", N);
  Kokkos::View<double *, Kokkos::HostSpace> h_y("host y", N);

  // Initialize data
  Kokkos::parallel_for(N,
    KOKKOS_LAMBDA (const size_t i) {
      x(i) = (i + 1) * 2.4;
      y(i) = (i + 1) * -1.2;
    });

  // Copy data to host for printing
  Kokkos::deep_copy(h_x, x);
  Kokkos::deep_copy(h_y, y);

  std::cout << "First and last elements before axpy: " << std::endl 
            << "x: " << h_x[0] << "," << h_x[N-1] << std::endl
            << "y: " << h_y[0] << "," << h_y[N-1] << std::endl;  

  // Apply axpy operation
  axpy(x, y, a);

  // Copy data to host for printing
  Kokkos::deep_copy(h_y, y);

  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << h_y[0] << "," << h_y[N-1] << std::endl;  

  }
  Kokkos::finalize();
}
