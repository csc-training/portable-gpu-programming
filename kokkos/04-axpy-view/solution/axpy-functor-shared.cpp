// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

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

  Kokkos::View<double *, Kokkos::SharedSpace> x("x", N);
  Kokkos::View<double *, Kokkos::SharedSpace> y("y", N);

  // Initialize data
  Kokkos::parallel_for(N,
    KOKKOS_LAMBDA (const size_t i) {
      x(i) = (i + 1) * 2.4;
      y(i) = (i + 1) * -1.2;
    });
  Kokkos::fence();

  std::cout << "First and last elements before axpy: " << std::endl 
            << "x: " << x(0) << "," << x(N-1) << std::endl
            << "y: " << y(0) << "," << y(N-1) << std::endl;  

  // Apply axpy operation
  axpy(x, y, a);
  Kokkos::fence();

  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << y(0) << "," << y(N-1) << std::endl;  

  }
  Kokkos::finalize();
}
