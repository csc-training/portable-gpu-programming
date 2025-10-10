#include <stdio.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Set device per rank
    int count, device;
    count = omp_get_num_devices();
    omp_set_default_device(rank % count);
    device = omp_get_default_device();

    printf("Hello from MPI rank %d/%d with GPU %d/%d\n", rank, size, device, count);

    // Data
    const int N = 1024*1024;
    double *data = (double*)malloc(N * sizeof(double));

#pragma omp target data map(alloc:data[0:N])
{

    if (rank == 0) {
        // Calculate data on rank 0
        #pragma omp target teams distribute parallel for
        for (int i = 0; i < N; i++) {
            data[i] = 42.0;
        }

        // Send with rank 0
        #pragma omp target data use_device_ptr(data)
        {
            MPI_Send(data, N, MPI_DOUBLE, 1, 123, MPI_COMM_WORLD);
        }

        printf("Rank %d sent\n", rank);

    } else if (rank == 1) {
        // Receive with rank 1
        #pragma omp target data use_device_ptr(data)
        {
            MPI_Recv(data, N, MPI_DOUBLE, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        #pragma omp target update from(data[0:N])
        printf("Rank %d received %f .. %f\n", rank, data[0], data[N-1]);
    }

}
    free(data);

    MPI_Finalize();

    return 0;
}

