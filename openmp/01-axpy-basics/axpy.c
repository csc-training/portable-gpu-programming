#include <stdio.h>

// Array size
#ifndef N
#define N 102400
#endif

// Max number of elements printed
#define PRINT_N 4
void print_array(const char* name, const double* x, size_t n);

int main(void)
{
    printf("Using N = %d\n", N);

    double alpha, x[N], y[N];

    // Initialization of values
    alpha = 3.0;
    for (int i = 0; i < N; i++) {
        double frac = 1.0 / ((double) (N - 1));
        x[i] = i * frac;
        y[i] = i * frac * 100;
    }

    // Print values
    printf("Input:\n");
    printf("a = %8.4f\n", alpha);
    print_array("x", x, N);
    print_array("y", y, N);

    // Calculate axpy
    // TODO: Use OpenMP pragma to execute this loop on GPU
    for (int i = 0; i < N; i++) {
        y[i] += alpha * x[i];
    }

    // Print values
    printf("Output:\n");
    print_array("y", y, N);
    return 0;
}


void print_array(const char* name, const double* x, size_t n)
{
    printf("%s =", name);
    for (size_t i = 0; i < n; i++) {
        if (i < PRINT_N || i > n - 1 - PRINT_N) {
            printf(" %8.4f", x[i]);
        } else if (i == PRINT_N) {
            printf(" ...");
        }
    }
    printf("\n");
}
