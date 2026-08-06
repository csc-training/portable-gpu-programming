/*
 * SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <math.h>

#ifdef TRACE
#include <roctx.h>
#else
#define roctxRangePush(...) ((void)0)
#define roctxRangePop(...) ((void)0)
#endif


static
void print_array(const char* name, const double* x, size_t n)
{
    const size_t PRINT_N = 4; // Limit the print size
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


static
void create_input(double *u, const int nx, const int ny, const double Lx, const double Ly)
{
    const double dx = Lx / (nx - 1);
    const double dy = Ly / (ny - 1);
    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) {
            int ij = i * nx + j;
            double x = j * dx - 0.5 * Lx;
            double y = i * dy - 0.5 * Ly;

            double acsch = asinh(1.0 / x);

            if (acsch > y && y > 0 && y < -0.75 * x + 2.75) {
                u[ij] = 100.0;
            } else if (acsch < y && y < 0 && y > -0.75 * x - 2.75) {
                u[ij] = -100.0;
            } else {
                u[ij] = 0.0;
            }
        }
    }
}


static
int write_array(const char *filename, const double *array, const size_t nx, const size_t ny, const double Lx, const double Ly)
{
    roctxRangePush(__func__);

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Failed to open file");
        roctxRangePop();
        return 1;
    }

    // Write the box size
    fwrite(&Lx, sizeof(double), 1, file);
    fwrite(&Ly, sizeof(double), 1, file);

    // Write the array size
    fwrite(&nx, sizeof(size_t), 1, file);
    fwrite(&ny, sizeof(size_t), 1, file);

    // Write the array layout (0 = row-major / C order)
    const unsigned char layout = 0;
    fwrite(&layout, 1, 1, file);

    // Write the array data
    const size_t count = nx * ny;
    size_t written = fwrite(array, sizeof(double), count, file);

    fclose(file);

    if (written != count) {
        fprintf(stderr, "Failed to write all elements to file\n");
        roctxRangePop();
        return 2;
    }

    roctxRangePop();

    return 0;
}
