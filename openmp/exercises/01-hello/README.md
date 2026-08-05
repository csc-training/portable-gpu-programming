<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Hello world with OpenMP offload

In this exercise we practise the first steps of OpenMP offload and runtime debugging.

A minimal example code is provided: The program first prints a hello message in host CPU,
and then a hello message from GPU.

The [solution directory](solution/) contains discussion on the tasks below.

## Tasks

1. Use `module` command to prepare the environment for compiling GPU code,
   following the general instructions for [Roihu](../../../README_Roihu.md).

2. Compile the C version:

       nvc -mp=gpu -O3 -gpu=cc90 -Wall hello.c -o hello.x

   or the Fortran version:

       nvfortran -mp=gpu -O3 -gpu=cc90 -Wall hello.F90 -o hello.x

3. Run the program using the job script `job.sh`:

       sbatch job.sh

4. How do we know if the code is using a GPU?

   Enable runtime debugging with an environment variable in `job.sh`:

       export NVCOMPILER_ACC_NOTIFY=3

   The output should have debugging lines like below:

       launch CUDA kernel ...

   If you don't see this lines, then correct modules are not loaded or the code is executed on CPU nodes.

   Can you identify the number of blocks and threads per block used for the kernel execution on GPU?

   The integer value of `NVCOMPILER_ACC_NOTIFY` is interpreted as a bit mask
   (see [documentation](https://docs.nvidia.com/hpc-sdk/archive/25.1/compilers/hpc-compilers-user-guide/index.html#using-openmp))
   and most verbose runtime debug information is obtained with `NVCOMPILER_ACC_NOTIFY=$((0x1F))`.

5. Insert `teams` and/or `parallel` directives in the target region to create multiple
   teams and/or threads.

   How many blocks and threads per block are created on the GPU?


## Bonus tasks: Cray compiler on LUMI

1. Use `module` command to prepare the environment for compiling GPU code,
   following the general instructions for [LUMI](../../../README_LUMI.md).

2. Compile the C version:

       CC -fopenmp -O3 -Wall hello.c -o hello.x

   or the Fortran version:

       ftn -fopenmp -O3 hello.F90 -o hello.x

3. Run the program using a job script for LUMI.

4. Runtime debug information is obtained with `CRAY_ACC_DEBUG`:

       export CRAY_ACC_DEBUG=2

   Can you identify the number of blocks and threads per block used for the kernel execution on GPU?

   Most verbose runtime debug information is obtained with `CRAY_ACC_DEBUG=3`.


## Bonus tasks: AMD Clang compiler on LUMI

1. Instead of the Cray compiler, try out AMD Clang compiler.
   Use the same GPU modules as above.

2. Compile the code with AMD Clang:

       amdclang++ -g -fopenmp -O3 --offload-arch=gfx90a hello.cpp -o hello.x

   Note that here we need to set the offload target architecture explictly.
   This is in contrast to the Cray compiler that decides the offload target based on the loaded environment.

3. Run the program using the job script.

   Now, `CRAY_ACC_DEBUG` doesn't give you any debugging output as it's specific for Cray compiler.
   Define `LIBOMPTARGET_INFO` instead (see [documentation](https://openmp.llvm.org/design/Runtimes.html#libomptarget-info)):

       export LIBOMPTARGET_INFO=$((0x10 | 0x20))

   Can you identify the number of blocks and threads per block used for the kernel execution on GPU?

   Most verbose runtime debug information is obtained with `LIBOMPTARGET_INFO=-1`.

   Try out also setting `LIBOMPTARGET_KERNEL_TRACE=1` or `LIBOMPTARGET_KERNEL_TRACE=2` instead of `LIBOMPTARGET_INFO` and examine the output
   (see [documentation](https://rocm.docs.amd.com/projects/llvm-project/en/latest/conceptual/openmp.html#environment-variables)).

