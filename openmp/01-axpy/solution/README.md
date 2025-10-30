<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

## Discussion

Note for instructors: use `run*.sh` scripts to generate all the output files.

### Exercises: Offload to GPU

1. See the solution code.

2. We enable runtime debugging and run the program:

       export CRAY_ACC_DEBUG=2
       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./axpy.x

   Outputs are in `axpy_{c,f}_debug2.out` for C and Fortran, respectively.

   The debugging outputs show memory allocations and transfers as well as the execution of
   the kernel using 400 blocks and 256 threads per block, that is, 102400 threads in total.

3. For `CRAY_ACC_DEBUG=1` outputs are in `axpy_{c,f}_debug1.out`
   and for `CRAY_ACC_DEBUG=3` in `axpy_{c,f}_debug3.out`.

   `CRAY_ACC_DEBUG=2` gives a good balance for the amount of details for the purposes of this training.

4. Files `axpy_{c,f}.lst` are attached for C and Fortran, respectively.
   The parallelized loop is marked in the output.

5. None of the other variations work expectedly. Either results are wrong (thread blocks are running for the same array indices instead of distributing the loop)
   or only a single thread might be used (very inefficient).


### Exercises: Offload to CPU threads

1. We execute the code:

       export OMP_DISPLAY_AFFINITY=true
       srun -p debug --nodes=1 --ntasks-per-node=1 --cpus-per-task=4 -t 0:10:00 ./axpy.x

   Outputs are in `axpy_{c,f}_cpu.out` for C and Fortran, respectively.

   The output shows that the program created 4 threads and that the threads were bound to individual cores.


### Bonus exercises: AMD Clang compiler

1. We execute the code:

       export LIBOMPTARGET_INFO=$((0x10 | 0x20))
       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./axpy.x

   Output is in `axpy_c_clang_debug.out`.

   We can read the memory transfers and kernel execution with 400 blocks and 256 threads per block from this output.

   Output for `LIBOMPTARGET_INFO=-1` is in `axpy_c_clang_debug_max.out`.

   Output for `LIBOMPTARGET_KERNEL_TRACE=1` (`unset LIBOMPTARGET_INFO`) is in `axpy_c_clang_debug_trace1.out`
   and for  `LIBOMPTARGET_KERNEL_TRACE=2` in `axpy_c_clang_debug_trace2.out`.


### Bonus exercises: NVIDIA HPC compiler on Mahti

1. We execute the code:

       export OMP_TARGET_OFFLOAD=MANDATORY
       export NVCOMPILER_ACC_NOTIFY=$((0x1 | 0x2))
       srun -p gputest --nodes=1 --ntasks-per-node=1 --cpus-per-task=4 --gres=gpu:a100:1 -t 0:10:00 ./axpy.x

   Output from C version:

       upload CUDA data  file=.../axpy.c function=main line=27 device=0 threadid=1 variable=y[:] bytes=819200
       upload CUDA data  file=.../axpy.c function=main line=27 device=0 threadid=1 variable=x[:] bytes=819200
       launch CUDA kernel file=.../axpy.c function=main line=27 device=0 host-threadid=0 num_teams=0 thread_limit=0 kernelname=nvkernel_main_F1L27_2 grid=<<<800,1,1>>> block=<<<128,1,1>>> shmem=0b
       download CUDA data  file=.../axpy.c function=main line=33 device=0 threadid=1 variable=x[:] bytes=819200
       download CUDA data  file=.../axpy.c function=main line=33 device=0 threadid=1 variable=y[:] bytes=819200
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   Output from Fortran version:

       upload CUDA data  file=.../axpy.F90 function=axpy line=31 device=0 threadid=1 variable=descriptor bytes=128
       upload CUDA data  file=.../axpy.F90 function=axpy line=31 device=0 threadid=1 variable=y(:) bytes=819200
       upload CUDA data  file=.../axpy.F90 function=axpy line=31 device=0 threadid=1 variable=descriptor bytes=128
       upload CUDA data  file=.../axpy.F90 function=axpy line=31 device=0 threadid=1 variable=x(:) bytes=819200
       launch CUDA kernel file=.../axpy.F90 function=axpy line=31 device=0 host-threadid=0 num_teams=0 thread_limit=0 kernelname=nvkernel_MAIN__F1L31_2_ grid=<<<800,1,1>>> block=<<<128,1,1>>> shmem=0b
       download CUDA data  file=.../axpy.F90 function=axpy line=35 device=0 threadid=1 variable=x(:) bytes=819200
       download CUDA data  file=.../axpy.F90 function=axpy line=35 device=0 threadid=1 variable=y(:) bytes=819200
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   We can read the memory transfers and kernel execution with 800 blocks and 128 threads per block from this output.

   Now `NVCOMPILER_ACC_NOTIFY=$((0x1F))`.
   Output from C version:

       Enter enter data construct file=.../axpy.c function=main line=27 device=0 threadid=1
       create CUDA data  bytes=819200 file=.../axpy.c function=main line=27 device=0 threadid=1
       alloc  CUDA data  devaddr=0x7fff0b2fa000 bytes=819200 file=.../axpy.c function=main line=27 device=0 threadid=1
       upload CUDA data  file=.../axpy.c function=main line=27 device=0 threadid=1 variable=y[:] bytes=819200
       create CUDA data  bytes=819200 file=.../axpy.c function=main line=27 device=0 threadid=1
       alloc  CUDA data  devaddr=0x7fff0b800000 bytes=819200 file=.../axpy.c function=main line=27 device=0 threadid=1
       upload CUDA data  file=.../axpy.c function=main line=27 device=0 threadid=1 variable=x[:] bytes=819200
       Leave enter data .../axpy.c main:27 device=0 threadid=1
       launch CUDA kernel file=.../axpy.c function=main line=27 device=0 host-threadid=0 num_teams=0 thread_limit=0 kernelname=nvkernel_main_F1L27_2 grid=<<<800,1,1>>> block=<<<128,1,1>>> shmem=0b
       Enter exit data construct file=.../axpy.c function=main line=27 device=0 threadid=1
       download CUDA data  file=.../axpy.c function=main line=33 device=0 threadid=1 variable=x[:] bytes=819200
       delete CUDA data  devaddr=0x7fff0b800000 bytes=819200 file=.../axpy.c function=main line=33 device=0 threadid=1
       download CUDA data  file=.../axpy.c function=main line=33 device=0 threadid=1 variable=y[:] bytes=819200
       delete CUDA data  devaddr=0x7fff0b2fa000 bytes=819200 file=.../axpy.c function=main line=33 device=0 threadid=1
       Implicit wait  file=.../axpy.c function=main line=33 device=0 threadid=1 queue=acc_async_sync
       Leave exit data .../axpy.c main:33 device=0 threadid=1
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   Output from Fortran version:

       Enter enter data construct file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       create CUDA data  bytes=819200 file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       alloc  CUDA data  devaddr=0x7fff0b2fa000 bytes=819200 file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       create CUDA data  bytes=128 file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       alloc  CUDA data  devaddr=0x7fff0b3c2000 bytes=512 file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       upload CUDA data  file=.../axpy.F90 function=axpy line=31 device=0 threadid=1 variable=descriptor bytes=128
       upload CUDA data  file=.../axpy.F90 function=axpy line=31 device=0 threadid=1 variable=y(:) bytes=819200
       create CUDA data  bytes=819200 file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       alloc  CUDA data  devaddr=0x7fff0b800000 bytes=819200 file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       create CUDA data  bytes=128 file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       alloc  CUDA data  devaddr=0x7fff0b3c2200 bytes=512 file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       upload CUDA data  file=.../axpy.F90 function=axpy line=31 device=0 threadid=1 variable=descriptor bytes=128
       upload CUDA data  file=.../axpy.F90 function=axpy line=31 device=0 threadid=1 variable=x(:) bytes=819200
       Leave enter data .../axpy.F90 axpy:31 device=0 threadid=1
       launch CUDA kernel file=.../axpy.F90 function=axpy line=31 device=0 host-threadid=0 num_teams=0 thread_limit=0 kernelname=nvkernel_MAIN__F1L31_2_ grid=<<<800,1,1>>> block=<<<128,1,1>>> shmem=0b
       Enter exit data construct file=.../axpy.F90 function=axpy line=31 device=0 threadid=1
       download CUDA data  file=.../axpy.F90 function=axpy line=35 device=0 threadid=1 variable=x(:) bytes=819200
       delete CUDA data  devaddr=0x7fff0b800000 bytes=819200 file=.../axpy.F90 function=axpy line=35 device=0 threadid=1
       delete CUDA data  devaddr=0x7fff0b3c2200 bytes=512 file=.../axpy.F90 function=axpy line=35 device=0 threadid=1
       download CUDA data  file=.../axpy.F90 function=axpy line=35 device=0 threadid=1 variable=y(:) bytes=819200
       delete CUDA data  devaddr=0x7fff0b2fa000 bytes=819200 file=.../axpy.F90 function=axpy line=35 device=0 threadid=1
       delete CUDA data  devaddr=0x7fff0b3c2000 bytes=512 file=.../axpy.F90 function=axpy line=35 device=0 threadid=1
       Implicit wait  file=.../axpy.F90 function=axpy line=35 device=0 threadid=1 queue=acc_async_sync
       Leave exit data .../axpy.F90 axpy:35 device=0 threadid=1
       Using N = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   Compiling with diagnostics:

       nvc -O3 -mp=gpu -gpu=cc80 -Minfo=mp axpy.c -o axpy.x
       nvfortran -O3 -mp=gpu -gpu=cc80 -Minfo=mp helper_functions.F90 axpy.F90 -o axpy.x

   Output for C:

       main:
            27, #omp target teams distribute parallel for
                27, Generating "nvkernel_main_F1L27_2" GPU kernel
                31, Loop parallelized across teams and threads(128), schedule(static)
            27, Generating implicit map(tofrom:y[:],x[:])

   Output for Fortran:

       axpy:
            31, !$omp target teams distribute parallel do
                31, Generating "nvkernel_MAIN__F1L31_2" GPU kernel
            31, Generating implicit map(tofrom:y(:),x(:))
