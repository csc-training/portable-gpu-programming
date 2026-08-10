/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "heat_helper_functions.h"


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

    // Dependency object for ensuring that data transfers and writing are serialized
    int write_flag;

    // Due to a bug in NVHPC compiler, we need to declare the reduction variables
    // outside the host-threaded scope
    double avg[4];

#pragma omp parallel num_threads(2)
#pragma omp single
{

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

        // Calculate average per quadrant
        if (it % 100 == 0) {
            const int nx2 = nx / 2;
            const int ny2 = ny / 2;

            #pragma omp task depend(out: avg[:4])
            {
                avg[0] = 0.0; avg[1] = 0.0; avg[2] = 0.0; avg[3] = 0.0;
            }

            #pragma omp target nowait map(tofrom: avg[0]) depend(in: u[0:nx*ny]) depend(inout:avg[0])
            #pragma omp teams distribute parallel for collapse(2) reduction(+:avg[0])
            for (int i = 0; i < ny2; i++) {
                for (int j = 0; j < nx2; j++) {
                    avg[0] += u[i * nx + j];
                }
            }

            #pragma omp target nowait map(tofrom: avg[1]) depend(in: u[0:nx*ny]) depend(inout:avg[1])
            #pragma omp teams distribute parallel for collapse(2) reduction(+:avg[1])
            for (int i = 0; i < ny2; i++) {
                for (int j = nx2; j < nx; j++) {
                    avg[1] += u[i * nx + j];
                }
            }

            #pragma omp target nowait map(tofrom: avg[2]) depend(in: u[0:nx*ny]) depend(inout:avg[2])
            #pragma omp teams distribute parallel for collapse(2) reduction(+:avg[2])
            for (int i = ny2; i < ny; i++) {
                for (int j = 0; j < nx2; j++) {
                    avg[2] += u[i * nx + j];
                }
            }

            #pragma omp target nowait map(tofrom: avg[3]) depend(in: u[0:nx*ny]) depend(inout:avg[3])
            #pragma omp teams distribute parallel for collapse(2) reduction(+:avg[3])
            for (int i = ny2; i < ny; i++) {
                for (int j = nx2; j < nx; j++) {
                    avg[3] += u[i * nx + j];
                }
            }

            // Print in a separate host thread
            #pragma omp task firstprivate(it) depend(in: avg[:4])
            {
                printf("%06d:  %+9.4f  %+9.4f  %+9.4f  %+9.4f\n", it,
                       avg[0] / (ny2 * nx2),
                       avg[1] / (ny2 * (nx - nx2)),
                       avg[2] / ((ny - ny2) * nx2),
                       avg[3] / ((ny - ny2) * (nx - nx2)));
            }
        }

        // Write data
        if (it % 1000 == 0) {
            #pragma omp target update from(u[0:nx*ny]) depend(in: u[0:nx*ny]) depend(inout:write_flag)

            // Write in a separate host thread
            #pragma omp task firstprivate(it, u) depend(inout:write_flag)
            {
                char filename[20];
                sprintf(filename, "u_%06d.bin", it);
                write_array(filename, u, nx, ny, Lx, Ly);
            }
        }

    }

#pragma omp taskwait

} // implicit wait at the end of the data clause

} // end of host threads

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
