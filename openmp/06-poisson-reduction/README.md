## Reduction operation

In this exercise we practise reduction operations.

Thus far we have missed one important part from our Jacobi code: convergence check.
In this exercise we add a converge check for every 10th iteration.

The current code is not very efficient though as it is calculating the norm on CPU.

We need to fix this.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercise: Offload to GPU

1. Instead of calculating the norm on CPU, calculate it on GPU.

   You can use the following sanity checks on command line:

       diff u000000.bin u000100.bin  # These files should differ
       diff u000500.bin u_end.bin  # These files should be the same (no output from diff)

   You can also use the attached Jupyter notebook to visualize the data dumps to check that they make sense.
