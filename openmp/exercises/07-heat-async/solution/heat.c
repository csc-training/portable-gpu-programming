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
    // Grid size
    const int nx = n, ny = n;
    const int n2 = nx * ny;

    // Box size
    const double Lx = 8.0;
    const double Ly = 8.0;

    // Diffusivity
    const double alpha = 0.5;

    // Grid spacing
    const double dx = Lx / (nx - 1);
    const double dy = Ly / (ny - 1);
    const double dx2 = dx * dx;
    const double dy2 = dy * dy;

    // Largest stable time step
    const double dt = dx2 * dy2 / (2.0 * alpha * (dx2 + dy2));

    // Print inputs
    printf("Inputs: n = %d, niter = %d\n", n, niter);
    printf("Diffusivity: %.2f\n", alpha);
    printf("Box: %.2f x %.2f discretized with grid spacing %.2e x %.2e\n", Lx, Ly, dx, dy);
    printf("Time propagation until %.2e with time step %.2e\n", dt * niter, dt);

    const double rx = alpha * dt / dx2;
    const double ry = alpha * dt / dy2;

    double *u, *unew;
    u = (double*)malloc(n2 * sizeof(double));
    unew = (double*)malloc(n2 * sizeof(double));

    // Initialize arrays
    create_input(u, nx, ny, Lx, Ly);
    memset(unew, 0, n2 * sizeof(double));

    // Write initial arrays
    write_array("u_initial.bin", u, nx, ny, Lx, Ly);

    // Propagate in time
    double t0 = omp_get_wtime();

#pragma omp target data map(tofrom: u[0:nx*ny]) map(to: unew[0:nx*ny])
{

    for (int it = 1; it < niter + 1; it++) {

        // Stencil update
        #pragma omp target nowait depend(in: u[0:nx*ny]) depend(out: unew[0:nx*ny])
        #pragma omp teams distribute parallel for collapse(2)
        for (int i = 1; i < ny - 1; i++) {
            for (int j = 1; j < nx - 1; j++) {
                int ij = i * nx + j;
                int ip = (i + 1) * nx + j;
                int im = (i - 1) * nx + j;
                int jp = i * nx + j + 1;
                int jm = i * nx + j - 1;
                unew[ij] = u[ij] + rx * (u[jp] - 2 * u[ij] + u[jm]) + ry * (u[ip] - 2 * u[ij] + u[im]);
            }
        }

        // Swap the arrays
        double *tmp = u;
        u = unew;
        unew = tmp;

        // Calculate average
        if (it % 100 == 0) {
            double average = 0.0;
            #pragma omp target map(tofrom: average) depend(in: u[0:nx*ny])
            #pragma omp teams distribute parallel for collapse(2) reduction(+:average)
            for (int i = 0; i < ny; i++) {
                for (int j = 0; j < nx; j++) {
                    int ij = i * nx + j;
                    average += u[ij];
                }
            }
            average /= (nx * ny);

            printf("%06d: %+e\n", it, average);
        }

    }

} // implicit wait at the end of the data clause

    double t1 = omp_get_wtime();

    // Write final result
    int i = (ny - 1) / 2, j = (nx - 1) / 2;
    printf("u[%d,%d] = %f\n", i, j, u[i * nx + j]);
    printf("Time spent: %.3f s\n", t1 - t0);
    write_array("u_final.bin", u, nx, ny, Lx, Ly);

    free(unew);
    free(u);
}


int main(int argc, char *argv[])
{
    // Default values
    int n = 1024;
    int niter = 500;
    int nrep = 3;

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
    if (argc > 3) {
        nrep = atoi(argv[3]);
        if (nrep < 1) {
            printf("Number of repetitions need to be greater than zero.\n");
            return 1;
        }
    }

    for (int i = 0; i < nrep; i++) {
        printf("RUN %d\n", i);
        run(n, niter);
        fflush(stdout);
    }

    return 0;
}
