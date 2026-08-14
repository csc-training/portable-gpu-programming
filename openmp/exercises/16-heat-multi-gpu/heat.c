/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <mpi.h>
#include "heat_helper_functions.h"


static inline
int calculate_inner_size(const int n_full, const int rank, const int ntasks) {
    const int n_full_inner = n_full - 2;  // Remove global boundary condition
    return n_full_inner / ntasks + (rank < n_full_inner % ntasks);
}

static inline
int calculate_comm_count(const int nx_full, const int ny_full, const int rank, const int ntasks) {
    const int ny_inner = calculate_inner_size(ny_full, rank, ntasks);
    int comm_count = nx_full * ny_inner;
    if (rank == 0) {
        // Communicate also global boundary in first
        comm_count += nx_full;
    }
    if (rank == ntasks - 1) {
        // Communicate also global boundary in last rank
        // Note! Different if so that it works correctly with ntasks=1
        comm_count += nx_full;
    }

    return comm_count;
}


void run(const int n, const int niter)
{
    // Grid size
    const int nx_full = n, ny_full = n;

    int ntasks, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int nghbrs[2] = {rank-1, rank+1};
    if (rank == 0) nghbrs[0] = MPI_PROC_NULL;
    if (rank == ntasks - 1) nghbrs[1] = MPI_PROC_NULL;

    const int nx = nx_full;
    const int ny_inner = calculate_inner_size(ny_full, rank, ntasks);
    const int ny = ny_inner + 2;  // Add halo and/or boundary conditions to the array

    // Box size
    const double Lx = 8.0;
    const double Ly = 8.0;

    // Diffusivity
    const double alpha = 0.5;

    // Grid spacing
    const double dx = Lx / (nx_full - 1);
    const double dy = Ly / (ny_full - 1);
    const double dx2 = dx * dx;
    const double dy2 = dy * dy;

    // Largest stable time step
    const double dt = dx2 * dy2 / (2.0 * alpha * (dx2 + dy2));

    if (rank == 0) {
        // Print inputs
        printf("Inputs: n = %d, niter = %d\n", n, niter);
        printf("Diffusivity: %.2f\n", alpha);
        printf("Box: %.2f x %.2f discretized with grid spacing %.2e x %.2e\n", Lx, Ly, dx, dy);
        printf("Time propagation until %.2e with time step %.2e\n", dt * niter, dt);
    }

    const double rx = alpha * dt / dx2;
    const double ry = alpha * dt / dy2;

    const int bytes = nx * ny * sizeof(double);
    double *u;
    u = (double*)malloc(bytes);
    memset(u, 0, bytes);

    // Prepare communication pointers and sizes
    int *comm_counts = NULL;
    int *comm_displs = NULL;
    double *u_comm;
    if (rank == 0) {
        // Communicate also first line (global boundary) in first rank
        u_comm = u;
    } else {
        // Skip first line (halo) in other ranks
        u_comm = u + nx;
    }
    int comm_count = calculate_comm_count(nx_full, ny_full, rank, ntasks);

    // Debug printing for communication
    if (rank == 0) {
        printf("Debug printing from each rank:\n");
        fflush(stdout);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printf("rank: %4d: displ = %12d, count = %12d, nx = %12d, ny = %12d\n", rank, u_comm - u, comm_count, nx, ny);
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        // Initialize arrays
        double *u_full = (double*)malloc(nx_full * ny_full * sizeof(double));
        create_input(u_full, nx_full, ny_full, Lx, Ly);

        // Write initial arrays
        write_array("u_initial.bin", u_full, nx_full, ny_full, Lx, Ly);

        // Calculate sizes to communicate to each rank
        comm_counts = (int*)malloc(ntasks * sizeof(int));
        comm_displs = (int*)malloc(ntasks * sizeof(int));
        comm_displs[0] = 0;
        for (int r = 0; r < ntasks; r++) {
            comm_counts[r] = calculate_comm_count(nx_full, ny_full, r, ntasks);
            if (r > 0) {
                comm_displs[r] = comm_displs[r-1] + comm_counts[r-1];
            }
        }

        // Debug printing for communication
        printf("Debug printing from root:\n");
        for (int r = 0; r < ntasks; r++) {
            printf("root: %4d: displ = %12d, count = %12d\n", r, comm_displs[r], comm_counts[r]);
        }

        // Scatter initial array
        MPI_Scatterv(u_full, comm_counts, comm_displs, MPI_DOUBLE,
                     u_comm, comm_count, MPI_DOUBLE,
                     0, MPI_COMM_WORLD);
        free(u_full);
    } else {
        // Scatter initial array
        MPI_Scatterv(NULL, NULL, NULL, MPI_DOUBLE,
                     u_comm, comm_count, MPI_DOUBLE,
                     0, MPI_COMM_WORLD);
    }
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    double *unew = (double*)malloc(bytes);
    memset(unew, 0, bytes);

    // Propagate in time
    double t0 = omp_get_wtime();

#pragma omp target data map(tofrom: u[0:nx*ny]) map(to: unew[0:nx*ny])
{
    for (int it = 1; it < niter + 1; it++) {

        // Halo exchange
        // TODO: communicate halo lines between MPI tasks

        // Stencil update
        #pragma omp target
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

    }

} // implicit wait at the end of the data clause

    double t1 = omp_get_wtime();

    free(unew);

    // Write final result
    if (rank == 0) {
        double *u_full = (double*)malloc(nx_full * ny_full * sizeof(double));

        // Gather the array
        MPI_Gatherv(u_comm, comm_count, MPI_DOUBLE,
                    u_full, comm_counts, comm_displs, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);

        int i = (ny_full - 1) / 2, j = (nx_full - 1) / 2;
        printf("u[%d,%d] = %f\n", i, j, u_full[i * nx_full + j]);
        printf("Time spent: %.3f s\n", t1 - t0);

        // Write final array
        write_array("u_final.bin", u_full, nx_full, ny_full, Lx, Ly);

        free(u_full);

        free(comm_counts);
        free(comm_displs);
    } else {
        // Gather the array
        MPI_Gatherv(u_comm, comm_count, MPI_DOUBLE,
                    NULL, NULL, NULL, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);
    }

    free(u);
}


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char node_name[MPI_MAX_PROCESSOR_NAME];
    int node_name_len;
    MPI_Get_processor_name(node_name, &node_name_len);

    // Set device per rank
    // TODO: set different device for each MPI task
    int device = omp_get_default_device();

    printf("MPI rank %d has GPU %d on node %s\n", rank, device, node_name);
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

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
        if (niter < 0) {
            printf("Number of iterations need to be non-negative.\n");
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
        if (rank == 0) printf("RUN %d\n", i);
        run(n, niter);
        if (rank == 0) fflush(stdout);
    }

    MPI_Finalize();

    return 0;
}
