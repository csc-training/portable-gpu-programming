/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include "helper_functions.h"

// Array size
#ifndef N
#define N 102400
#endif

int main(void)
{
    printf("Using N = %d\n", N);

    double x[N], y[N];

    // Initialization
    for (int i = 0; i < N; i++) {
        double frac = 1.0 / ((double) (N - 1));
        x[i] = i * frac;
        y[i] = i * frac * 100;
    }

    // Print input values
    printf("Input:\n");
    print_array("x", x, N);
    print_array("y", y, N);

    // Calculate dot product
    double sum = 0.0;
    //#pragma omp target
    //#pragma omp teams distribute parallel for
    //#pragma omp target map(tofrom: sum)
    //#pragma omp teams distribute parallel for
    #pragma omp target map(tofrom: sum)
    #pragma omp teams distribute parallel for reduction(+:sum)
    for (int i = 0; i < N; i++) {
        sum += x[i] * y[i];
    }

    // Print output values
    printf("Output:\n");
    printf("sum = %.4f\n", sum);
    return 0;
}
