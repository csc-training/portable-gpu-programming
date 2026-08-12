<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Using GPUs and MPI with OpenMP offload

In this exercise you can practice building and running a program combining OpenMP offload 
and MPI. You will see also demonstration on how to map tasks to GPUs.

## Tasks for Roihu

1. Load the nvhpc module. Then, the code can be compiled with the mpi wrappers:

       mpicc -O3 -mp=gpu -gpu=cc90 hello.c -o hello.x
       mpif90 -O3 -mp=gpu -gpu=cc90 hello.F90 -o hello.x

   Run the program on two nodes with two tasks and two GPUs on each node:

       --nodes=2 --ntasks-per-node=2 --gres=gpu:gh200:2

   Alternatively, you can request only a single GPU but two MPI tasks, in which case both tasks are using the same GPU:

       --ntasks-per-node=2 --gres=gpu:gh200:1

## Tasks for LUMI

1. Study, compile, and run the demo code.

   Cray wrapper includes MPI headers and libraries automatically, so the code can be compiled
   as before:

       cc -O3 -fopenmp hello.c -o hello.x
       ftn -O3 -fopenmp hello.F90 -o hello.x

   Run the program on two nodes with two tasks and two GPUs on each node:

       --nodes=2 --ntasks-per-node=2 --gpus-per-node=2

   Alternatively, you can request only a single GPU but two MPI tasks, in which case both tasks are using the same GPU:

       --ntasks-per-node=2 --cpus-per-task=4 --gpus-per-node=1

