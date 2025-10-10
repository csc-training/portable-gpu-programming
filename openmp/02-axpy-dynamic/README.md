## The axpy kernel with dynamic arrays

In this exercise we start looking on data mapping clauses.

Starting from the model solution of the previous exercise, we have now modified the code so that
the array size is not a compile time constant but instead read from a command line argument.

We have moved the `main()` function with all the command line argument parsing
to the bottom of the file. The function `run()` in the top of the file is of most interest for us.

The code can be compiled for serial execution as before:

    cc -O3 axpy.c -o axpy.x

Run the program on a single CPU core for a given array size (5 in this example):

    srun -p debug --nodes=1 --ntasks-per-node=1 --cpus-per-task=1 -t 0:10:00 ./axpy.x 5

Expected output:

    Using N = 5
    Input:
    a =   3.0000
    x =   0.0000   0.2500   0.5000   0.7500   1.0000
    y =   0.0000  25.0000  50.0000  75.0000 100.0000
    Output:
    y =   0.0000  25.7500  51.5000  77.2500 103.0000

The issue is that on GPU this code crashes now!

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercises

1. Compile and run this code on GPUs like in the previous exercise.
   Remember to load the correct GPU modules and use `-fopenmp` compilation flag.

   Code compiles fine, but it crashes during execution to memory access fault although
   the same pragma was working earlier. :(

   Use runtime debugging environment variables introduced in the previous exercise
   (`CRAY_ACC_DEBUG` or `LIBOMPTARGET_INFO`) to find the issue and then fix the code.
