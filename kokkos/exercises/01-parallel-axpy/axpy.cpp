// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <iostream>

int main(int argc, char** argv)
{

  const double a = 0.5;
  constexpr size_t N = 100;

  double *x = (double *) malloc(N * sizeof(double));
  double *y = (double *) malloc(N * sizeof(double));

  // Initialize x and y
  for (size_t i = 0; i < N; i++) 
  {
    x[i] = (i + 1) * 2.4;
    y[i] = (i + 1) * -1.2;
  }
  
  std::cout << "First and last elements before axpy: " << std::endl
            << "x: " << x[0] << "," << x[N-1] << std::endl
            << "y: " << y[0] << "," << y[N-1] << std::endl;

  // Apply axpy operation
  for (size_t i = 0; i < N; i++) 
  {
    y[i] += a * x[i]; 
  }
 
  // Check results
  std::cout << "First and last element (both should be zero):" << std::endl 
            << y[0] << "," << y[N-1] << std::endl;  

}
