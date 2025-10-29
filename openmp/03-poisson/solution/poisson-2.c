/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "helper_functions.h"


void run(const int n, const int niter)
{
    printf("Using n = %d, niter = %d\n", n, niter);

    char filename[20];
    const int nx = n, ny = n;
    const int n2 = nx * ny;

    double h2, *f, *u, *unew;
    f = (double*)malloc(n2 * sizeof(double));
    u = (double*)malloc(n2 * sizeof(double));
    unew = (double*)malloc(n2 * sizeof(double));
    h2 = 1.0;

    // Initialize arrays
    create_input(f, nx, ny);
    memset(u, 0, n2 * sizeof(double));
    memset(unew, 0, n2 * sizeof(double));

    // Write initial arrays
    sprintf(filename, "u%06d.bin", 0);
    write_array(filename, u, n2);
    write_array("f.bin", f, n2);

    // Iterate
    double t0 = omp_get_wtime();

#pragma omp target data map(to: f[0:nx*ny]) map(tofrom: u[0:nx*ny]) map(to: unew[0:nx*ny])
{

    for (int it = 1; it < niter + 1; it++) {

        // Stencil update
        #pragma omp target teams distribute parallel for
        for (int i = 1; i < ny - 1; i++) {
            for (int j = 1; j < nx - 1; j++) {
                int ind = i * nx + j;
                int ip = (i + 1) * nx + j;
                int im = (i - 1) * nx + j;
                int jp = i * nx + j + 1;
                int jm = i * nx + j - 1;
                unew[ind] = 0.25 * (u[ip] + u[im] + u[jp] + u[jm] - h2 * f[ind]);
            }
        }

        // Swap the arrays
        double *tmp = u;
        u = unew;
        unew = tmp;
    }

}

    double t1 = omp_get_wtime();

    // Write final result
    int i = ny / 2, j = nx / 2;
    printf("u[%d,%d] = %f\n", i, j, u[i * nx + j]);
    printf("Time spent: %.3f s\n", t1 - t0);
    write_array("u_end.bin", u, n2);

    free(unew);
    free(u);
    free(f);
}


int main(int argc, char *argv[])
{
    // Default values
    int n = 1024;
    int niter = 500;

    if (argc > 1) {
        n = atoi(argv[1]);
        if (n < 1) {
            printf("Size needs to be greater than zero.\n");
            return 1;
        }
    }
    if (argc > 2) {
        niter = atoi(argv[2]);
        if (niter < 1) {
            printf("Number of iterations need to be greater than zero.\n");
            return 1;
        }
    }

    run(n, niter);

    return 0;
}
