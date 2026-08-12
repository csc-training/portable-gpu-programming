/*
 * SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);
    int ntasks;
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // find name of the processor (node) that this rank is running on.
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int processor_name_length;

    MPI_Get_processor_name(processor_name, &processor_name_length);

    // Determine the node local rank
    int nodeRank;
    MPI_Comm commNode;
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &commNode);
    MPI_Comm_rank(commNode, &nodeRank);

    // Find out number of GPUs and assign each rank to a GPU
    int deviceCount = omp_get_num_devices();
    omp_set_default_device(nodeRank % deviceCount);
    int my_device = omp_get_default_device();

    printf("Rank %d on node %s: %d GPUs in total, using GPU %d\n",
           rank, processor_name, deviceCount, my_device);


    MPI_Finalize();
}
