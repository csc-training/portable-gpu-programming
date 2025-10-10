#include <stdio.h>
#include <stdlib.h>
#include <hipblas/hipblas.h>
#include "helper_functions.h"

void run(const int n)
{
    printf("Using n = %d\n", n);

    // Create handle for hipblas
    hipblasHandle_t handle;
    hipblasCreate(&handle);
    hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_HOST);

    double alpha, *x, *y;
    x = (double*)malloc(n * sizeof(double));
    y = (double*)malloc(n * sizeof(double));

    // Initialization
    alpha = 3.0;
    for (int i = 0; i < n; i++) {
        double frac = 1.0 / ((double) (n - 1));
        x[i] = i * frac;
        y[i] = i * frac * 100;
    }

    // Print input values
    printf("Input:\n");
    printf("a = %8.4f\n", alpha);
    print_array("x", x, n);
    print_array("y", y, n);

    // Calculate axpy
    #pragma omp target data map(to: x[0:n]) map(tofrom: y[0:n])
    #pragma omp target data use_device_ptr(x, y)
    hipblasDaxpy(handle, n, &alpha, x, 1, y, 1);

    // Print output values
    printf("Output:\n");
    print_array("y", y, n);

    free(y);
    free(x);

    // Destroy hipblas handle
    hipblasDestroy(handle);
}


int main(int argc, char *argv[])
{
    // Array size
    int n = 102400;

    if (argc > 1) {
        n = atoi(argv[1]);
        if (n < 1) {
            printf("Size needs to be greater than zero.\n");
            return 1;
        }
    }

    run(n);

    return 0;
}
