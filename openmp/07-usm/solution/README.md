## Discussion

### Exercises for axpy

1. Running the code without data mapping statements works fine with XNACK enabled:

       export HSA_XNACK=1
       export CRAY_ACC_DEBUG=2
       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./axpy.x

   Output:

       ACC: Version 6.0 of HIP already initialized, runtime version 60032831
       ACC: Get Device 0
       ACC: Set Thread Context
       ACC: Start transfer 2 items from axpy.c:30
       ACC:       no allocate for unified memory 'y' (0 bytes)
       ACC:       no allocate for unified memory 'x' (0 bytes)
       ACC: End transfer (to acc 0 bytes, to host 0 bytes)
       ACC: Execute kernel __omp_offloading_73ac72ce_1f01e992_run_l30_cce$noloop$form blocks:400 threads:256 from axpy.c:30
       ACC: Start transfer 2 items from axpy.c:30
       ACC:       no free for unified memory 'x' (0 bytes)
       ACC:       no free for unified memory 'y' (0 bytes)
       ACC: End transfer (to acc 0 bytes, to host 0 bytes)
       Using n = 102400
       Input:
       a =   3.0000
       x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
       y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
       Output:
       y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

   Note the prints about unified memory. Data is not transferred by the OpenMP runtime but
   it takes place via the page migration mechanism under the hood.

### Exercises for Jacobi

1. Running the provided code without changes with default settings:

       export HSA_XNACK=1
       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./poisson.x

   Output:

       RUN 0
       Using n = 1024, niter = 500
       000100: 2566.940171
       000200: 2560.999477
       000300: 2555.732335
       000400: 2550.477719
       000500: 2545.235544
       u[512,512] = -120.808781
       Time spent:  1.442 s
       RUN 1
       Using n = 1024, niter = 500
       000100: 2508.884536
       000200: 2445.440013
       000300: 2383.823907
       000400: 2323.976953
       000500: 2265.842343
       u[512,512] = -120.808781
       Time spent:  0.745 s
       RUN 2
       Using n = 1024, niter = 500
       000100: 2566.940171
       000200: 2560.999477
       000300: 2555.074825
       000400: 2549.166011
       000500: 2543.272924
       u[512,512] = -120.808781
       Time spent:  0.728 s

   We note that results change randomly, indicating a data race, and that the code is much slower than before.

   Reference output without USM:

       RUN 0
       Using n = 1024, niter = 500
       000100: 2508.884536
       000200: 2445.440013
       000300: 2383.823907
       000400: 2323.976953
       000500: 2265.842343
       u[512,512] = -120.808781
       Time spent:  0.771 s
       RUN 1
       Using n = 1024, niter = 500
       000100: 2508.884536
       000200: 2445.440013
       000300: 2383.823907
       000400: 2323.976953
       000500: 2265.842343
       u[512,512] = -120.808781
       Time spent:  0.024 s
       RUN 2
       Using n = 1024, niter = 500
       000100: 2508.884536
       000200: 2445.440013
       000300: 2383.823907
       000400: 2323.976953
       000500: 2265.842343
       u[512,512] = -120.808781
       Time spent:  0.023 s

   Something seems to be broken with the asynchronous execution and code works by removing `nowait`.
   We can remove all data mapping statements except `map` seems to be needed for the correctness
   of the reduction call.

   Note also that overlapping the I/O step with computation would also need some more code as now
   there is no separate data buffers for CPU and GPU.

   In the example code, we have also updated the memory allocations to be aligned to rule out that
   from affecting the performance issue.

   Note that you could get better performance by replacing `malloc()` and `free()` with

       f = (double*)omp_target_alloc(nbytes, omp_get_default_device());
       u = (double*)omp_target_alloc(nbytes, omp_get_default_device());
       unew = (double*)omp_target_alloc(nbytes, omp_get_default_device());
       ...
       omp_target_free(unew, omp_get_default_device());
       omp_target_free(u, omp_get_default_device());
       omp_target_free(f, omp_get_default_device());

   but ideally this kind of changes wouldn't be needed, but the compiler would do the right thing.
