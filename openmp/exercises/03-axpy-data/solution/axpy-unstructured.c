// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include "helper_functions.h"


int main(int argc, char* argv[]) {
    // Array size
    int n = 102400;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    printf("Array size n = %d\n", n);

    double alpha, *x, *y;
    x = (double*)malloc(n * sizeof(double));
    y = (double*)malloc(n * sizeof(double));

    #pragma omp target enter data map(alloc: x[0:n], y[0:n])

    // Initialization
    alpha = 3.0;
    #pragma omp target teams distribute parallel for
    for (int i = 0; i < n; i++) {
        double frac = 1.0 / ((double) (n - 1));
        x[i] = i * frac;
        y[i] = i * frac * 100;
    }

    // Print input values
    printf("Input:\n");
    printf("a = %8.4f\n", alpha);
    #pragma omp target update from(x[0:n]) from(y[0:n])
    print_array("x", x, n);
    print_array("y", y, n);

    // Calculate axpy
    #pragma omp target teams distribute parallel for
    for (int i = 0; i < n; i++) {
        y[i] += alpha * x[i];
    }

    #pragma omp target exit data map(delete: x[0:n]) map(from: y[0:n])

    // Print output values
    printf("Output:\n");
    print_array("y", y, n);

    free(y);
    free(x);

    return 0;
}
