// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

void evolve(double *unew, const double *u,
            const int nx, const int ny,
            const double rx, const double ry)
{
    #pragma omp target
    #pragma omp teams distribute parallel for
    for (int i = nx; i < (ny - 1) * nx; i++) {
        int m = i % nx;
        if (m > 0 && m < nx - 1) {
            unew[i] = u[i] + rx * (u[i+1] - 2 * u[i] + u[i-1])
                           + ry * (u[i+nx] - 2 * u[i] + u[i-nx]);
        }
    }
}
