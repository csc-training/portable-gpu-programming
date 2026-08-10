<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Declaring device functions for OpenMP offload

In this exercise we practise declaring device functions.

We use the axpy code from a previous exercise, which
has been split to two compilation units:
1. The main program in `main.{c,F90}`
2. The GPU kernels in `kernels.{c,F90}`

These codes are provided in c/fortran directories together with a makefile.

The [solution directory](solution/) contains a model solution and discussion on the tasks below.

## Tasks

1. Try to compile the codes using `make`. The linking is expected to fail to an error:

   C:

       lld: error: undefined symbol: axpy(double, double, double)

   Fortran:

       lld: error: undefined symbol: axpy$kernels_

   The reason for this error is that the axpy kernel in `kernels.{c,F90}` has not been
   compiled for device execution.

2. Insert suitable `declare target` directives to fix the compilation.
