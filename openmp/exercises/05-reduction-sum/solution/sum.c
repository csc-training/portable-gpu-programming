// SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char* argv[])
{
    // Array size
    int n = 100000;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    printf("Array size: %d\n", n);

    // Start timing
    double t0 = omp_get_wtime();

    // Calculate sum
    double total = 0;
    #pragma omp target map(tofrom: total)
    #pragma omp teams distribute parallel for reduction(+:total)
    for (int i = 0; i < n; i++) {
        total += sin((double)i);
    }

    // End timing
    double t1 = omp_get_wtime();

    printf("Sum: %f\n", total);
    printf("Calculation took %.3f milliseconds\n", (t1 - t0) * 1e3);

    return 0;
}
