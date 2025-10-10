## First GPU kernel: axpy

In this exercise we practise the basics of OpenMP offload and runtime debugging by using a code performing the axpy operation.

The axpy operation is a fundamental linear algebra operation defined as

$$
y_i \leftarrow \alpha x_i + y_i
$$

where $\alpha$ is a scalar and $x$ and $y$ are vectors of the same size.

The provided code can be used as follows.

Compile the code:

    cc -O3 axpy.c -o axpy.x

Run the program on a single CPU core:

    srun -p debug --nodes=1 --ntasks-per-node=1 --cpus-per-task=1 -t 0:10:00 ./axpy.x

Expected output:

    Using N = 102400
    Input:
    a =   3.0000
    x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
    y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
    Output:
    y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

Note that you can change the array size at compile time as follows:

    cc -O3 -DN=5 axpy.c -o axpy.x

Expected output:

    Using N = 5
    Input:
    a =   3.0000
    x =   0.0000   0.2500   0.5000   0.7500   1.0000
    y =   0.0000  25.0000  50.0000  75.0000 100.0000
    Output:
    y =   0.0000  25.7500  51.5000  77.2500 103.0000

We want to accelerate this code with GPU!

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercises: Offload to GPU

1. Offload the axpy operation to GPU by inserting a suitable OpenMP target pragma (see 'TODO' in the code).
   Use the following GPU modules:

       ml LUMI/24.03
       ml partition/G
       ml rocm/6.0.3

   Compile the code:

       cc -O3 -fopenmp axpy.c -o axpy.x

   Run the program on a single GPU. We allocate also 7 CPU cores per task as
   as every LUMI-G node has 7 CPU cores per a MI250X GCD. These CPU cores can be
   used by the OpenMP runtime:

       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./axpy.x

   The output should be correct, that is, the same as above.

2. How do we know if the code is using a GPU?

   Enable runtime debugging with an environment variable and run the program again:

       export CRAY_ACC_DEBUG=2
       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./axpy.x

   The output should have many lines starting with 'ACC:', something like below:

       ACC: Version 6.0 of HIP already initialized, runtime version 60032831
       ACC: Get Device 0
       ACC: Set Thread Context
       ...

   Study the output.

   How many blocks and threads per block are used for the kernel execution on GPU?
   Are these values reasonable? Hint: we would expect that N=102400 threads are executed in total.

   Can you identify memory transfers?

3. Examine how the debugging output changes if you set `CRAY_ACC_DEBUG` to 1 or 3 instead of 2.

4. Compile the code with diagnostics:

       cc -O3 -fopenmp -fsave-loopmark axpy.c -o axpy.x

   This creates a file 'axpy.lst'.
   Can you deduce from the file what was done for the axpy loop?

   See [HPE Cray Clang C and C++ Quick Reference (17.0.1)](https://support.hpe.com/hpesc/public/docDisplay?docId=dp00004439en_us)
   for further information.

5. Try out what happens if you have an incorrect omp pragma. That is, if instead of
   `#pragma omp target teams distribute parallel for` you would have, e.g.,
   `#pragma omp target` or `#pragma omp target teams` etc.

   - Do you get correct results?
   - Do you expect that the resulting number of blocks and threads per block give good performance?


### Exercises: Offload to CPU threads

1. Launch a **new terminal session** for CPU execution and load the following CPU modules:

       ml LUMI/24.03
       ml partition/C

   Cray compiler wrappers choose the offload target based on the loaded modules that set specific environment variables,
   so now with these modules loaded, the same compilation command will target CPU threads:

       cc -O3 -fopenmp axpy.c -o axpy.x

   Run the program on a CPU partition with 4 threads:

       srun -p debug --nodes=1 --ntasks-per-node=1 --cpus-per-task=4 -t 0:10:00 ./axpy.x

   How do we know now that the code is run with multiple threads?
   You can set an environment variable to display thread affinities:

       export OMP_DISPLAY_AFFINITY=true
       srun -p debug --nodes=1 --ntasks-per-node=1 --cpus-per-task=4 -t 0:10:00 ./axpy.x

   Can you deduce from the output how many threads were used?


### Bonus exercises: AMD Clang compiler

1. Instead of the Cray compiler, try out AMD Clang compiler.
   Use the same GPU modules as above:

       ml LUMI/24.03
       ml partition/G
       ml rocm/6.0.3

   Compile the code with AMD Clang:

       amdclang -g -O3 -fopenmp --offload-arch=gfx90a axpy.c -o axpy.x

   Note that here we set the offload target architecture explictly.
   This is in contrast to the Cray compiler that decides the offload target based on the loaded environment.

   Now, `CRAY_ACC_DEBUG` doesn't give you any debugging output as it's specific for Cray compiler.
   Define `LIBOMPTARGET_INFO` instead (see [documentation](https://openmp.llvm.org/design/Runtimes.html#libomptarget-info)):

       export LIBOMPTARGET_INFO=$((0x10 | 0x20))
       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./axpy.x

   Study the output.

   How many blocks and threads per block are used for the kernel execution on GPU?

   Can you identify memory transfers?

   Try also out the output with the most verbose setting `LIBOMPTARGET_INFO=-1`.

   Try out also setting `LIBOMPTARGET_KERNEL_TRACE=1` or `LIBOMPTARGET_KERNEL_TRACE=2` instead of `LIBOMPTARGET_INFO` and examine the output
   (see [documentation](https://rocm.docs.amd.com/projects/llvm-project/en/latest/conceptual/openmp.html#environment-variables)).

### Bonus exercises: loop construct

1. Try out the `target teams loop` construct. Note that it might not work with all compilers unless you split the pragma to two lines:

       #pragma omp target teams
       #pragma omp loop
