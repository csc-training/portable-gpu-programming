#include <Kokkos_Core.hpp>
#include <iostream>

int main(int argc, char** argv)
{
  Kokkos::initialize(argc, argv);

  const double a = 0.5;
  constexpr int N = 100;

  double *x = (double *) malloc(N * sizeof(double));
  double *y = (double *) malloc(N * sizeof(double));

  // Initialize;
  for (int i = 0; i < N; i++) {
    x[i] = i*1.2;
    y[i] = -i*1.2;
  };

  // axpy with normal loop
  for (int i=0; i < N; i++) // Iteration range
  {
    y[i] += a * x[i]; // computational body
  }

  std::cout << y[1] << " " << y[N-1] << std::endl;  

  // reset
  for (int i = 0; i < N; i++) {
    x[i] = i*1.2;
    y[i] = -i*1.2;
  };

  // axpy with functor
  struct axpy {
    double *_x, *_y;
    const double _a;
    axpy(double *x, double *y, double a) :
      _x(x), _y(y), _a(a) {};
    void operator()(const int i) const {
      _y[i] += _a * _x[i];
    }
  };

  Kokkos::parallel_for(N, axpy(x, y, a));

  std::cout << y[1] << " " << y[N-1] << std::endl;  

  // reset
  for (int i = 0; i < N; i++) {
    x[i] = i*1.2;
    y[i] = -i*1.2;
  };

  // axpy with lambda
  Kokkos::parallel_for(N,
     [=] (const int i) {
          y[i] += a * x[i];
     });

  std::cout << y[1] << " " << y[N-1] << std::endl;  

  Kokkos::finalize();
}
