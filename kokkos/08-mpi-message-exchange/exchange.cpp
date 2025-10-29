// SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <vector>
#include <mpi.h>

int main(int argc, char *argv[])
{
    constexpr int arraysize = 100000;
    constexpr int msgsize =   100000;
    std::vector<int> message(arraysize);
    std::vector<int> receiveBuffer(arraysize);

    int rank, ntasks;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (ntasks < 2)
    {
        printf("Please run with at least 2 MPI processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Initialize message and receive buffer
    for (int i = 0; i < arraysize; i++)
    {
        message[i] = rank;
        receiveBuffer[i] = -1;
    }

    int tag = 0;
    // Use modulo for obtaining dst and sr
    int dst = (rank + 1) % ntasks;
    int src = (rank - 1 + ntasks) % ntasks;

    MPI_Sendrecv(message.data(), msgsize, MPI_INT, dst, tag,
                 receiveBuffer.data(), msgsize, MPI_INT, src, tag, 
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("Rank %i received %i elements, first %i\n", rank, msgsize, receiveBuffer[0]);

    MPI_Finalize();
    return 0;
}
