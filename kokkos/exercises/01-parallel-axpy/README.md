<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# axpy operation with Kokkos parallel dispatch

In this exercise you can practice implementing the basic "axpy" operation
$y = a \times x + y$ in parallel using Kokkos. In order to focus only on
parallel dispatch, the proper way to manage memory with Kokkos is not considered yet.

## Tasks 

The code [axpy.cpp](axpy.cpp) implements the basic "axpy" operation
in standard C/C++. Replace the `for` loops by corresponding Kokkos parallel operations 
and the computational body by lambda function.

1. Build and run the code in LUMI with OpenMP backend. You can use your own Kokkos installation
or use the module
```
ml use /scratch/project_462001074/modulefile
ml kokkos/omp
```

2. Build and run the code in LUMI with HIP backend. You can use your own Kokkos installation
or use the module
```
ml use /scratch/project_462001074/modulefile
ml kokkos/hip
```
   - Can you explain why the code fails at run time?
   - In AMD systems one can control automatic data migration between host and device memories
     with `HSA_XNACK` environment variable
   - Try to set `export HSA_XNACK=1` before running the code, does it work now?

3. Build and run the code in Mahti with OpenMP backend. You can use your own Kokkos installation
or use the module
```
ml use /scratch/project_2015315/modulefiles
ml kokkos/omp
```

4. Build and run the code in Mahti with cuda backend. You can use your own Kokkos installation
or use the module
```
ml use /scratch/project_2015315/modulefiles
ml kokkos/cuda
```
   - NVIDIA A100 GPUs do not support automatic data migration for memory allocated with `malloc`.
   - In next lecture we look how to manage memory in portable way with Kokkos

