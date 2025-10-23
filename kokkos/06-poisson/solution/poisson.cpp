#include <Kokkos_Core.hpp>
#include <iostream>
#include <cstdio>
#include <cmath>

#define NX 1024
#define NY 1024

// Initialize 2d array with Gaussian
template <typename T>
void init(T x)
{
  int nx = x.extent(0);
  int ny = x.extent(1);
  double cx = nx / 2.0;
  double cy = ny / 2.0;
  double sigma2 = 0.05 * nx*ny;  // Width of the Gaussian
  double kx = 20.0 / nx;  // Spatial frequency in x
  double ky = 10.0 / ny;  // Spatial frequency in y

  Kokkos::parallel_for("init",
    Kokkos::MDRangePolicy<Kokkos::Rank<2> >({0, 0}, {nx, ny}),
      KOKKOS_LAMBDA(const int i, const int j) {
        double dx = j - cx;
        double dy = i - cy;
        double r2 = dx * dx + dy * dy;
        x(i, j) = cos(kx * dx + ky * dy) * exp(-r2 / sigma2);
      });
}
 
void run(const int n, const int niter)
{

  const int nx = n, ny = n;
  Kokkos::View<double**> f("f", nx, ny);
  Kokkos::View<double**> u("u", nx, ny);
  Kokkos::View<double**> unew("unew", nx, ny);
  double h2 = 1.0;

  Kokkos::deep_copy(u, 0.0);
  Kokkos::deep_copy(unew, 0.0);
  init(f);

  Kokkos::Timer timer;
  double t0 = timer.seconds();

  // Jacobi iteration
  #pragma nounroll
  for (int iter = 0; iter < niter; iter++) {
    Kokkos::parallel_for("jacobi",
      Kokkos::MDRangePolicy<Kokkos::Rank<2> >({1, 1}, {nx-1, ny-1}),
        KOKKOS_LAMBDA(const int i, const int j) {
          unew(i, j) = 0.25 * (u(i-1, j) + u(i+1, j) + u(i, j-1) + u(i, j+1) - h2 * f(i, j));
        });

    Kokkos::fence();

    std::swap(u, unew);
  }

  auto unew_host = Kokkos::create_mirror_view(unew);
  Kokkos::deep_copy(unew_host, unew);

  FILE *file = fopen("u.bin", "wb");
  size_t count = nx*ny;
  fwrite(&count, sizeof(size_t), 1, file);
  // Write the data
  size_t written = fwrite(unew_host.data(), sizeof(double), count, file);
  fclose(file);

  // Check the result
  double mean = 0.0;
  Kokkos::parallel_reduce("reduce",
    Kokkos::MDRangePolicy<Kokkos::Rank<2> >({1, 1}, {nx-1, ny-1}),
        KOKKOS_LAMBDA(const int i, const int j, double& mean) {
           mean += unew(i,j);
        }, mean);

  mean /= ((nx - 1) * (ny - 1));

  double t1 = timer.seconds();
  double elapsed_seconds = t1 - t0;

  int i = ny / 2, j = nx / 2;
  printf("u[%d,%d] = %f\n", i, j, unew_host(i, j));
  printf("Mean u = %f\n", mean);
  printf("Time spent: %6.3f s\n", elapsed_seconds);

}

int main(int argc, char *argv[])
{
  Kokkos::initialize();
  // Array size
  int n = 1024;

  // Number of iterations
  int niter = 500;

  if (argc > 2) {
      niter = std::atoi(argv[2]);
      if (niter < 1) {
          printf("Number of iterations need to be greater than zero.\n");
          return 1;
      }
  }
  if (argc > 1) {
      n = std::atoi(argv[1]);
      if (n < 1) {
          printf("Size needs to be greater than zero.\n");
          return 1;
      }
  }

  run(n, niter);

  Kokkos::finalize();
  return 0;
}

