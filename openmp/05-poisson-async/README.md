## Asynchronous kernel execution

In this exercise we practise asynchronous kernel execution.

We start from the previous code that writes the array on every 100th iteration to the disk.

The kernel launches are synchronous by default in OpenMP offload.
This means that the host process is waiting for the GPU kernel to finish before it proceeds with the host code.
So, the problem with our code is that the GPU is idling while the host is writing to disk.
Let's fix this!

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercise: Offload to GPU

1. We have added a roctx timer to the `write_array()` function for measuring the time spent in this function.

   In order to compile this code, link to the roctx library:

       cc -fopenmp poisson.c -lroctx64 -o poisson.x

   And this is how you can run it with a profiler:

       srun -p dev-g --nodes=1 --ntasks-per-node=1 --gpus-per-node=1 -t 0:10:00 rocprof --hip-trace --roctx-trace ./poisson.x

   This creates a file `results.json`. Transfer this file to your computer, and open it in ui.perfetto.dev.

   You should see in the profile kernel launches, data transfers, host hip calls, and `write_array()` functions.

2. Based on the knowledge learned from the profile, improve the code by utilizing asynchronous kernel launches to
   overlap the `write_array()` function with GPU computation.

   Make sure that the results stay correct and that the files contain correct data too.

   You can check the files by `diff`:

       diff u000000.bin u000100.bin  # These files should differ
       diff u000100.bin u000100_old_file_from_synchronous_code.bin  # These files should be exactly the same

   Do you get any speed up by overlapping computation and I/O?
