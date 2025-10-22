## Asynchronous kernel execution

In this exercise we practise asynchronous kernel execution.

We start from the previous code that writes the array on every 1000th iteration to the disk.

The kernel launches are synchronous by default in OpenMP offload.
This means that the host process is waiting for the GPU kernel to finish before it proceeds with the host code.
So, the problem with our code is that the GPU is idling while the host is writing to disk.
Let's fix this!

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercises

1. The `write_array()` helper function has timing capability with roctx library that can be enabled like this:

       cc -O3 -fopenmp poisson.c -DTRACE -lroctx64 -o poisson.x
       ftn -O3 -fopenmp helper_functions.F90 poisson.F90 -DTRACE -lroctx64 -o poisson.x

   And this is how you can run the code with a profiler (**always remove old outputs before running profiler!**):

       rm results.*; srun -p dev-g --nodes=1 --ntasks-per-node=1 --cpus-per-task=7 --gpus-per-node=1 -t 0:10:00 rocprof --hip-trace --roctx-trace ./poisson.x 4096 5000 1

   Note: we run here a bigger case (4096) so that the kernel execution is not too fast (takes more time than kernel launch)
   and we run only for one repetition to reduce the profile file size.

   This creates a file `results.json`. Transfer this file to your computer and open it in https://ui.perfetto.dev.

   You should see in the profile kernel launches, data transfers, host hip calls, and `write_array()` functions.

2. Based on the knowledge learned from the profile, improve the code by utilizing asynchronous kernel launches.

   First, add `nowait` clause with suitable `depend` clauses in the kernel to make the launch asynchronous.

   Then, restructure the host code to allow the execution of `write_array()` function to overlap with GPU computation.

   Make sure that the results stay correct and that the files contain correct data too.

   You can check that the files are the same within numerical accuracy:

       python3 compare.py u001000.bin u001000_old_file_from_synchronous_code.bin
       python3 compare.py u002000.bin u002000_old_file_from_synchronous_code.bin
       ...
       python3 compare.py u005000.bin u005000_old_file_from_synchronous_code.bin

   Do you get any speed up by overlapping computation and I/O?
