## Reduction operation

In this exercise we practise reduction operations.

Thus far we have missed one important part from our Poisson code: convergence check.
In this exercise we have add a converge check for every 100th iteration
to the model solution of the previous exercise.

The current code is not working though, it is calculating the norm on CPU usin wrong data
as all the real data is on GPU.

We need to fix this.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercises

1. Calculate the norm on GPU using reduction clause.

2. (Bonus) Use profiler to gain insight on the code behavior.

   In particular, is the I/O step still overlapping with GPU computation?

   If not, try to modify the code to keep the critical path (Jacobi iteration on GPU)
   as fast as possible.
