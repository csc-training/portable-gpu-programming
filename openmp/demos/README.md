# Demo

See exercises for further information about compilation and running.

## LUMI

1. Use the following GPU modules:

       ml LUMI/24.03
       ml partition/G
       ml rocm/6.0.3

   Compile the code:

       cc -O3 -fopenmp hello.c -o hello.x

   Run the program:

       srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 ./hello.x

2. Compile and run the different `hello-*.c` programs.
