// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

__global__ static
void evolve_kernel(double *unew, const double *u,
                   const int nx, const int ny,
                   const double rx, const double ry)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x + nx;

    if (i < (ny - 1) * nx) {
        int m = i % nx;
        if (m > 0 && m < nx - 1) {
            unew[i] = u[i] + rx * (u[i+1] - 2 * u[i] + u[i-1])
                           + ry * (u[i+nx] - 2 * u[i] + u[i-nx]);
        }
    }
}


extern "C" {

void evolve(double *unew, const double *u,
            const int nx, const int ny,
            const double rx, const double ry)
{
    int block = 128;
    int grid = ((ny - 2) * nx + block - 1) / block;
    evolve_kernel<<<grid, block>>>(unew, u, nx, ny, rx, ry);
    cudaDeviceSynchronize();
}

}


