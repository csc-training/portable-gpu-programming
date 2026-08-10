<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Heat equation: Integrate custom CUDA/HIP kernels

In this exercise we optimize the heat equation code with by integrating custom CUDA/HIP kernel.

For simplicity, the starting point is the heat equation code from the first exercise,
with only the time-evolution kernel.

This code has been split to two files:
1. The main program in `heat.{c,F90}`.
2. The GPU kernel `evolve()` implemented in different ways in `kernels*` files.
   The file `kernels.{c,F90}` corresponds to the code we have been using thus far.

The [solution directory](solution/) contains a model solution and discussion on the tasks below.

## Tasks

1. Study the split code and compile it:

       nvc -mp=gpu -O3 -gpu=cc90 -Wall kernels.c heat.c -o heat.x

   Check its runtime for reference on a larger case, e.g. `./heat.x 16384 1000`.

2. The example includes a CUDA/HIP implementation of the same kernel in `kernels.cu`.

   Use the provided Makefile by running `make clean; make` to compile this code.

   The code runs but results are incorrect!

   Fix this with `use_device_ptr` to ensure that GPU pointers are passed to the custom kernel.

   Is the performance improved by using the custom kernel?

3. (Bonus) Consider the original OpenMP kernel in `kernels.c`.
   The new field is calculated using double loop,
   which is rather unnecessary as the array is one-dimensional.

   Using this knowledge, rewrite the OpenMP kernel using a single one-dimensional loop,
   making sure that the boundary values are treated correctly.

   Check the performance of this new OpenMP kernel.

   The provided CUDA/HIP kernel is implemented in the same fashion.

   Hint: Note that

       ij = i * nx + j;
       ip = (i + 1) * nx + j;
       im = (i - 1) * nx + j;
       jp = i * nx + j + 1;
       jm = i * nx + j - 1;

   can be written as:

       ij = i * nx + j;
       ip = ij + nx;
       im = ij - nx;
       jp = ij + 1;
       jm = ij - 1;
