<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Matrix multiplication with Kokkos kernels

In this exercise you can practice how to use Kokkos kernels, as an example
we use standard matrix-matrix product.

## Tasks

Write a program that calculates the general matrix-matrix product
$C = \alpha * A * B + \beta C$ using the BLAS3 function gemm from Kokkos
kernels.

Try to evaluate the efficieny of the function by calculating the TFLOP/s
performance.  For $N x N$ matrices there are $2N^3$ operations, and you
can measure the time spent using `Kokkos::Timer`: 
```
Kokkos::Timer timer
double start_time = timer.seconds()
...
double stop_time = timer.seconds()
```

In LUMI Kokkos Kernels is available (only for the HIP backend) with
```
ml use /scratch/project_462001074/modulefiles
ml kokkos-kernels
```

In Mahti Kokkos Kernels is available (only for the CUDA backend) with
```
ml use /scratch/project_2015315/modulefiles
ml kokkos-kernels
```

