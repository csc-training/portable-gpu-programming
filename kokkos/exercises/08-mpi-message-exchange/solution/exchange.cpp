// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <Kokkos_Core.hpp>
#include <cstdio>
#include <vector>
#include <mpi.h>

int main(int argc, char *argv[])
{
    constexpr int arraysize = 100000;
    constexpr int msgsize =   100000;

    MPI_Init(&argc, &argv);
    Kokkos::initialize();
    {
    Kokkos::View<int[arraysize]> message("msg");
    Kokkos::View<int[arraysize]> receiveBuffer("buf");

    int rank, ntasks;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (ntasks < 2)
    {
        printf("Please run with at least 2 MPI processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Initialize message and receive buffer
    Kokkos::parallel_for(arraysize, KOKKOS_LAMBDA (const int i)
    {
        message[i] = rank;
        receiveBuffer[i] = -1;
    });

    int tag = 0;
    // Use modulo for obtaining dst and sr
    int dst = (rank + 1) % ntasks;
    int src = (rank - 1 + ntasks) % ntasks;

    MPI_Sendrecv(message.data(), msgsize, MPI_INT, dst, tag,
                 receiveBuffer.data(), msgsize, MPI_INT, src, tag, 
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    auto h_receiveBuffer = Kokkos::create_mirror_view_and_copy(Kokkos::HostSpace(), receiveBuffer);
    printf("Rank %i received %i elements, first %i\n", rank, msgsize, h_receiveBuffer[0]);

    }
    Kokkos::finalize();
    MPI_Finalize();
    return 0;
}
