// SPDX-FileCopyrightText: 2019 CSC - IT Center for Science Ltd. <www.csc.fi>
//
// SPDX-License-Identifier: MIT

#include <cstdio>
#include <vector>
#include <mpi.h>

int main(int argc, char *argv[])
{
    constexpr int arraysize = 100000;
    constexpr int msgsize =   100;
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

    if (rank == 0)
    {
        // Send total of 'msgsize' integers to rank 1.
        MPI_Send(message.data(), msgsize, MPI_INT, 1, tag, MPI_COMM_WORLD);
        MPI_Recv(receiveBuffer.data(), msgsize, MPI_INT, 1, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Rank %i received %i elements, first %i\n", rank, msgsize, receiveBuffer[0]);
    }
    else if (rank == 1)
    {
        MPI_Send(message.data(), msgsize, MPI_INT, 0, tag, MPI_COMM_WORLD);
        MPI_Recv(receiveBuffer.data(), msgsize, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Rank %i received %i elements, first %i\n", rank, msgsize, receiveBuffer[0]);
    }

    MPI_Finalize();
    return 0;
}
