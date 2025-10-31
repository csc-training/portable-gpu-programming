// SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>

#include <mpi.h>
#include <sycl/sycl.hpp>

template <typename T>
void init(sycl::queue &q, const size_t n, T &x)
{
    auto kernel = [=](sycl::id<1> i) {
        x[i] = 1.1 * (size_t)i;
    };

    q.parallel_for(n, kernel);
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    const size_t n = argc > 1 ? (size_t)std::stoll(argv[1]) : 1024;
    const size_t nbytes = sizeof(double) * n;

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto gpu_devices = sycl::device::get_devices(sycl::info::device_type::gpu);
    auto count = std::size(gpu_devices);
    auto device = gpu_devices[rank];
    sycl::queue q{device, sycl::property::queue::in_order{}};

    printf("Hello from MPI rank %d/%d with a GPU of %zu\n", rank, size, count);

    // Device data
    double *x = sycl::malloc_device<double>(n, q);
    q.fill(x, 0, n);
    q.wait();

    if (rank == 0) {
        // Initialize data on rank 0
        init(q, n, x);
        q.wait();

        // Send with rank 0
        MPI_Send(x, n, MPI_DOUBLE, 1, 123, MPI_COMM_WORLD);
        printf("Rank %d sent\n", rank);
    } else if (rank == 1) {
        // Receive with rank 1
        MPI_Recv(x, n, MPI_DOUBLE, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank %d received\n", rank);
    }

    // Copy result to CPU and print
    std::vector<double> h_x(n);
    q.memcpy(h_x.data(), x, nbytes).wait();
    std::stringstream ss;
    ss << "Rank " << rank << " has";
    for (int i = 0; i < std::min(8ul, n); ++i) ss << " " << h_x[i];
    if (n > 8) ss << " ...";
    ss << "\n";
    std::cout << ss.str();

    free(x, q);

    MPI_Finalize();
}
