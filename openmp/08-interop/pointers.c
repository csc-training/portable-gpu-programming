/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    const int n = 1024;
    double *x;
    x = (double*)malloc(n * sizeof(double));

    printf("printing from host the address of x in host: %p\n", x);

    #pragma omp target data map(to: x[0:n])
    {
        #pragma omp target data use_device_ptr(x)
        {
            printf("printing from host the address of x in dev:  %p\n", x);
        }

        #pragma omp target
        {
            printf("printing from dev  the address of x in dev:  %p\n", x);
        }
    }

    free(x);
}
