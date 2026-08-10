// SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <iostream>

template <typename T>
void axpy(T *x, T *y, T a, size_t n)
{
  // Apply axpy operation
  for (size_t i = 0; i < n; i++) 
  {
    y[i] += a * x[i]; 
  }
}

int main(int argc, char** argv)
{

  constexpr size_t N = 100;

  // double precision version
  const double ad = 0.5;

  std::cout << "Double precision version" << std::endl;

  double *xd = (double *) malloc(N * sizeof(double));
  double *yd = (double *) malloc(N * sizeof(double));

  // Initialize x and y
  for (size_t i = 0; i < N; i++) 
  {
    xd[i] = (i + 1) * 2.4;
    yd[i] = (i + 1) * -1.2;
  }
  
  std::cout << "First and last elements before axpy: " << std::endl
            << "x: " << xd[0] << "," << xd[N-1] << std::endl
            << "y: " << yd[0] << "," << yd[N-1] << std::endl;

  axpy(xd, yd, ad, N);
 
  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << yd[0] << "," << yd[N-1] << std::endl;  

  // single precision version
  const float af = 0.5;

  std::cout << "Single precision version" << std::endl;

  float *xf = (float *) malloc(N * sizeof(float));
  float *yf = (float *) malloc(N * sizeof(float));

  // Initialize x and y
  for (size_t i = 0; i < N; i++) 
  {
    xf[i] = (i + 1) * 2.4;
    yf[i] = (i + 1) * -1.2;
  }
  
  std::cout << "First and last elements before axpy: " << std::endl
            << "x: " << xf[0] << "," << xf[N-1] << std::endl
            << "y: " << yf[0] << "," << yf[N-1] << std::endl;

  axpy(xf, yf, af, N);
 
  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << yf[0] << "," << yf[N-1] << std::endl;  

  // integer version
  const int ai = 2;

  std::cout << "Integer version" << std::endl;

  int *xi = (int *) malloc(N * sizeof(int));
  int *yi = (int *) malloc(N * sizeof(int));

  // Initialize x and y
  for (size_t i = 0; i < N; i++) 
  {
    xi[i] = (i + 1) * -1;
    yi[i] = (i + 1) * 2;
  }
  
  std::cout << "First and last elements before axpy: " << std::endl
            << "x: " << xi[0] << "," << xi[N-1] << std::endl
            << "y: " << yi[0] << "," << yi[N-1] << std::endl;

  axpy(xi, yi, ai, N);
 
  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << yi[0] << "," << yi[N-1] << std::endl;  


}
