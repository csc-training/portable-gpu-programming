#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>
#include <chrono>

// Functor for getting linear index from two dimensional
struct Idx {
  int _nx;
  Idx(int nx) : _nx(nx) {};
  int operator()(const int i, const int j) const {return i * _nx + j;}
};

// Initialize 2d array with Gaussian
template <typename T>
void init(T &x, int nx, int ny)
{
  double cx = nx / 2.0;
  double cy = ny / 2.0;
  double sigma2 = 0.05 * nx*ny;  // Width of the Gaussian
  double kx = 20.0 / nx;  // Spatial frequency in x
  double ky = 10.0 / ny;  // Spatial frequency in y

  auto idx = Idx(nx);

  for (int i=0; i < nx; i++)
    for (int j=0; j < ny; j++) 
      {
        double dx = j - cx;
        double dy = i - cy;
        double r2 = dx * dx + dy * dy;
        x[idx(i,j)] = cos(kx * dx + ky * dy) * exp(-r2 / sigma2);
      }
}
 
void run(const int n, const int niter)
{
  printf("Using n = %d, niter = %d\n", n, niter);

  const int nx = n, ny = n;
  const int n2 = nx * ny;
  std::vector<double> f(n2), u(n2, 0.0), unew(n2, 0.0);

  double h2 = 1.0;

  init(f, nx, ny);

  auto idx = Idx(nx);

  using Clock = std::chrono::high_resolution_clock;
  auto t0 = Clock::now();

  // Jacobi iteration
  #pragma nounroll
  for (int iter = 1; iter < niter + 1; iter++) {
    for (int i=1; i < nx-1; i++)
      for (int j=1; j < ny-1; j++) 
        {
          unew[idx(i,j)] = 0.25 * (u[idx(i-1,j)] + u[idx(i+1,j)] + u[idx(i, j-1)] + u[idx(i,j+1)] - h2 * f[idx(i,j)]);
        }

    std::swap(u, unew);
  }

  FILE *file = fopen("u.bin", "wb");
  size_t count = nx*ny;
  fwrite(&count, sizeof(size_t), 1, file);
  // Write the data
  size_t written = fwrite(unew.data(), sizeof(double), count, file);
  fclose(file);

  auto t1 = Clock::now();

  // Check the result
  double mean = 0.0;
    for (int i=1; i < nx-1; i++)
      for (int j=1; j < ny-1; j++) 
        {
           mean += unew[idx(i,j)];
        }

  mean /= ((nx - 1) * (ny - 1));

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
  double elapsed_seconds = 1.0e-3 * duration.count();

  int i = ny / 2, j = nx / 2;
  printf("u[%d,%d] = %f\n", i, j, u[idx(i, j)]);
  printf("Mean u = %f\n", mean);
  printf("Time spent: %6.3f s\n", elapsed_seconds);

}

int main(int argc, char *argv[])
{
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

  return 0;
}

