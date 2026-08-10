<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Calculate axpy using GPU libraries

In this exercise we practice calling external GPU libraries from OpenMP offload.

GPU BLAS libraries expose nearly identical C APIs on NVIDIA (cuBLAS) and AMD (hipBLAS),
but with different header files, handle types, and constant names.
The thin wrapper header `gpublas.h` hides these differences behind a unified `blas_*` API,
so the same `axpy.c` compiles without changes on both platforms.

As the GPU libraries have often C interface, the Fortran case is a bit more
involved and requires the use of `iso_c_binding`.
Example bindings are provided in `gpublas_bindings.F90`.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

## Tasks

1. The code `axpy` tries to call axpy from cuBLAS/hipBLAS, but it's broken.

   On Roihu (NVIDIA), the code can be compiled with

       nvc -mp=gpu -O3 -gpu=cc90 -Wall -DCUDA axpy.c -L${NVHPC_ROOT}/math_libs/13.1/lib64 -lcublas -o axpy.x
       nvfortran -mp=gpu -O3 -gpu=cc90 -DCUDA axpy.F90 -L${NVHPC_ROOT}/math_libs/13.1/lib64 -lcublas -o axpy.x

   On LUMI (AMD), the code can be compiled with

       cc -O3 -fopenmp axpy.c -lhipblas -o axpy.x
       ftn -O3 -fopenmp axpy.F90 -lhipblas -o axpy.x

   Then, the code can be run on GPU as usual.

   In order to make the BLAS call work, we need to fix the data passed to the call
   (see 'TODO' in the code).

   Fix the code, using the `pointers` code from previous exercise as an example.
