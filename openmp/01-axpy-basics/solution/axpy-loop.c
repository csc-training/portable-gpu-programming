#include <stdio.h>
#include "helper_functions.h"

// Array size
#ifndef N
#define N 102400
#endif

int main(void)
{
    printf("Using N = %d\n", N);

    double alpha, x[N], y[N];

    // Initialization
    alpha = 3.0;
    for (int i = 0; i < N; i++) {
        double frac = 1.0 / ((double) (N - 1));
        x[i] = i * frac;
        y[i] = i * frac * 100;
    }

    // Print input values
    printf("Input:\n");
    printf("a = %8.4f\n", alpha);
    print_array("x", x, N);
    print_array("y", y, N);

    // Calculate axpy
    #pragma omp target teams
    #pragma omp loop
    for (int i = 0; i < N; i++) {
        y[i] += alpha * x[i];
    }

    // Print output values
    printf("Output:\n");
    print_array("y", y, N);
    return 0;
}
