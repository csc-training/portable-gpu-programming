## Reduction operation

In this exercise we practise reduction operations.

Thus far we have missed one important part from our Jacobi code: convergence check.
In this exercise we add a converge check for every 100th iteration.

The current code is not working, it's calculating the norm on CPU and with wrong data
as all real data is on GPU.

We need to fix this.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercises

1. Calculate the norm on GPU using reduction clause.

2. Use profiler to gain insight on the code behavior and improve the code if needed.
