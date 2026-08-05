<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

## Discussion

### Exercises

1. Using the Cray compiler and running with runtime debug:

       export CRAY_ACC_DEBUG=2
       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./axpy.x

   This gives this output for C version:

       ACC: Version 6.0 of HIP already initialized, runtime version 60032831
       ACC: Get Device 0
       ACC: Set Thread Context
       ACC: Start transfer 2 items from axpy.c:28
       ACC:       allocate 'y' (0 bytes)
       ACC:       allocate 'x' (0 bytes)
       ACC: End transfer (to acc 0 bytes, to host 0 bytes)
       Memory access fault by GPU node-4 (Agent handle: 0x3261c0) on address 0x7000. Reason: Unknown.

   Here 0 bytes are allocated for the x and y arrays on GPU!
   This doesn't seem right and the kernel execution crashes as there is no GPU memory allocated.

   The problem is that compiler wasn't able to deduce what is the correct size of x and y arrays.
   The fix is to add `map(to: x[0:n]) map(tofrom: y[0:n])` to the pragma to tell the compiler
   what are the ranges of arrays used in the kernel.

   With this we get the code working:

       ACC: Version 6.0 of HIP already initialized, runtime version 60032831
       ACC: Get Device 0
       ACC: Set Thread Context
       ACC: Start transfer 2 items from axpy.c:28
       ACC:       allocate, copy to acc 'y[0:n]' (819200 bytes)
       ACC:       allocate, copy to acc 'x[0:n]' (819200 bytes)
       ACC: End transfer (to acc 1638400 bytes, to host 0 bytes)
       ACC: Execute kernel __omp_offloading_73ac72ce_1f01be5b_run_l28_cce$noloop$form blocks:400 threads:256 from axpy.c:28
       ACC: Start transfer 2 items from axpy.c:28
       ACC:       free 'x[0:n]' (819200 bytes)
       ACC:       copy to host, free 'y[0:n]' (819200 bytes)
       ACC: End transfer (to acc 0 bytes, to host 819200 bytes)
       Using n = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   Note here that in contrast to the previous exercise, now the array x is not copied back to host after the kernel execution,
   because we defined it as 'to' in the map clause. Basically, there is no reason to copy it back as the GPU didn't write any new values to it.

   For Fortran version, the equivalent map clause is `map(to: x(1:n)) map(tofrom: y(1:n))`.
   Note that the compiler created this exactly same behavior by default.
