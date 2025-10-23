#include <iostream>
#include <vector>
#include <cstdio>
#include <cmath>

#define NX 1024
#define NY 1024

// C macros shouldn't really be used, but we want to use
// here 2D indexing to make porting to Kokkos more straightforward 
//#define IDX(i, j)  ((i) * (NX) + (j))

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
 
int main(int argc, char** argv)
{

  constexpr int niters = 500;

  int nx, ny;
  if (2 == argc) {
    nx = atoi(argv[1]);
    ny = atoi(argv[1]);
  } else {
     nx = NX;
     ny = NY;
  }

  std::vector<double> f(nx*ny), u(nx*ny, 0.0), unew(nx*ny, 0.0);

  double h2 = 1.0;

  init(f, nx, ny);
  auto idx = Idx(nx);

  // Jacobi iteration
  #pragma nounroll
  for (int iter = 0; iter < niters; iter++) {
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

  // Check the result
  double mean = 0.0;
    for (int i=1; i < nx-1; i++)
      for (int j=1; j < ny-1; j++) 
        {
           mean += unew[idx(i,j)];
        }

  mean /= ((nx - 1) * (ny - 1));

  int i = ny / 2, j = nx / 2;
  std::cout << "Center u " << unew[idx(i,j)] << std::endl;
  std::cout << "Mean u " << mean << std::endl;

  return 0;
}
