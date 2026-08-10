// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

void evolve(double *unew, const double *u,
            const int nx, const int ny,
            const double rx, const double ry)
{
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
}
