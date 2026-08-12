<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Send and receive messages between GPUs

In this exercise we practice using multiple GPUs through MPI.

It is possible to do MPI outside target regions via host as usual.
However, for avoiding unnecessary memory transfers between GPU and host
as well as for better performance, it is often better to use GPU-aware MPI.

In order to use GPU-aware MPI, we need to pass GPU pointers to the MPI calls,
that is, use `use_device_ptr` like in the previous exercice.

In addition, often device management is needed with MPI, e.g., setting
one GPU per task.

The provided code demonstrates both of these aspects.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

## Tasks for Roihu

1. Load the nvhpc module. Then, the code can be compiled with the mpi wrappers:

       mpicc -O3 -mp=gpu -gpu=cc90 mpi_send_and_recv.c -o mpi_send_and_recv.x
       mpif90 -O3 -mp=gpu -gpu=cc90 mpi_send_and_recv.F90 -o mpi_send_and_recv.x

   Run the code requesting two tasks and two GPUs:

       --ntasks-per-node=2 --cpus-per-task=4 --gres=gpu:gh200:2

   Alternatively, you can request only a single GPU but two MPI tasks, in which case both tasks are using the same GPU:

       --ntasks-per-node=2 --cpus-per-task=4 --gres=gpu:gh200:1

## Tasks for LUMI

1. Study, compile, and run the demo code.

   Cray wrapper includes MPI headers and libraries automatically, so the code can be compiled
   as before:

       cc -O3 -fopenmp mpi_send_and_recv.c -o mpi_send_and_recv.x
       ftn -O3 -fopenmp mpi_send_and_recv.F90 -o mpi_send_and_recv.x

   Run the code requesting two tasks and two GPUs:

       --ntasks-per-node=2 --cpus-per-task=4 --gpus-per-node=2

   Alternatively, you can request only a single GPU but two MPI tasks, in which case both tasks are using the same GPU:

       --ntasks-per-node=2 --cpus-per-task=4 --gpus-per-node=1

   For running, we need to also enable GPU-aware MPI:

       export MPICH_GPU_SUPPORT_ENABLED=1


