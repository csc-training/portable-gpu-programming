## Using multiple GPUs and MPI

In this exercise we practice combining MPI and OpenMP offload.

It is possible to do MPI outside target regions via host as usual.
However, for avoiding unnecessary memory transfers between GPU and host
as well as for better performance, it is often better to use GPU-aware MPI.

In order to use GPU-aware MPI, we need to pass GPU pointers to the MPI calls,
that is, use `use_device_ptr` like in the previous exercice.

In addition, often device management is needed with MPI, e.g., setting
one GPU per task.

The demo code demonstrates both of these aspects.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercises

1. Study, compile, and run the demo code.

   Cray wrapper includes MPI headers and libraries automatically, so the code can be compiled
   as before:

       cc -O3 -fopenmp mpi_send_and_recv.c -o mpi_send_and_recv.x
       ftn -O3 -fopenmp mpi_send_and_recv.F90 -o mpi_send_and_recv.x

   For running, we need to enable GPU-aware MPI and then run with two tasks:

       export MPICH_GPU_SUPPORT_ENABLED=1
       srun -p dev-g --nodes=1 --ntasks-per-node=2 --cpus-per-task=7 --gpus-per-node=2 -t 0:10:00 ./mpi_send_and_recv.x
