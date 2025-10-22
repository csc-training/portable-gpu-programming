## Solving 2D Poisson's equation with Jacobi iteration

In this exercise we practise the data mapping clauses across multiple GPU kernels.

Poisson's equation in 2D is

$$
\nabla^2 u(x, y) = f(x, y)
$$

where $$f$$ is a given function and $$u$$ is the function to be solved.

This equation can be discretized using finite differences as:

$$
\frac{u_{i+1,j} - 2u_{i,j} + u_{i-1,j}}{\Delta x^2} + \frac{u_{i,j+1} - 2u_{i,j} + u_{i,j-1}}{\Delta y^2} = f_{i,j}
$$

Assuming a uniform grid where $$\Delta x = \Delta y = h$$, this simplifies to:

$$
\frac{u_{i+1,j} + u_{i-1,j} + u_{i,j+1} + u_{i,j-1} - 4u_{i,j}}{h^2} = f_{i,j}
$$

Rearranging terms gives the standard five-point stencil:

$$
u_{i,j} = \frac{1}{4} \left( u_{i+1,j} + u_{i-1,j} + u_{i,j+1} + u_{i,j-1} - h^2 f_{i,j} \right)
$$

This discretized equation can be solved iteratively using various numerical methods, and in this exercise,
we solve it using Jacobi iteration that updates all grid points simultaneously using values from the previous iteration.

The algorithm uses the five-point stencil:

$$
u_{i,j}^{(k+1)} = \frac{1}{4} \left( u_{i+1,j}^{(k)} + u_{i-1,j}^{(k)} + u_{i,j+1}^{(k)} + u_{i,j-1}^{(k)} - h^2 f_{i,j} \right)
$$

where $$u_{i,j}^{(k)}$$ is the value of $$u$$ at grid point $$(i,j)$$ during the $$k$$-th iteration.

The algorithm comprises of the following steps:

1. Initialize the grid with an initial guess $$u_{i,j}^{(0)}$$ (zeros in the example code).
2. Iterate over all interior grid points and update $$u_{i,j}^{(k+1)}$$ using values from $$u^{(k)}$$.
3. Repeat until the solution converges, i.e., the difference between successive iterations is below a chosen tolerance.
   In this exercise, we iterate for a fixed number of iterations for simplicity.


Starting from the previous axpy code, we have now replaced the axpy calculation with the Jacobi iteration.
We have added timing using `omp_get_wtime()`, so even the serial code needs to be compiled with `-fopenmp`:

    cc -O3 -fopenmp poisson.c -o poisson.x
    ftn -O3 -fopenmp helper_functions.F90 poisson.F90 -o poisson.x

Run the program on a single CPU core for a an 1024x1024 array for 500 iterations:

    srun -p debug --nodes=1 --ntasks-per-node=1 --cpus-per-task=1 -t 0:10:00 ./poisson.x 1024 500

Expected output:

    Using n = 1024, niter = 500
    u[512,512] = -120.808781
    Time spent: 0.234 s

We want to speed this up by utilizing GPU!

The [solution directory](solution/) contains a model solution and discussion on the exercises below.


### Exercises

1. Offload Jacobi iteration to GPU by inserting a suitable OpenMP target directives around the loop (see 'TODO 1' in the code).
   Remember to include map clauses like in the previous exercise.

   Please ensure that the result remains correct.

   What is the speed up you get by using a single GPU over the serial CPU calculation?
   Or is the program actually slower?

2. Enable `CRAY_ACC_DEBUG=2` and examine the memory transfers during the execution.
   Insert separate data transfer clauses to ensure that there is no unnecessary copying back and forth between
   the CPU and GPU memories (see 'TODO 2' in the code).

   What is the speed up you get now?

3. Could you write the kernel launch pragmas (`target teams distribute parallel for`) in some other ways around the two loops?
   What kind of speed ups do you observe? Hint: try also `collapse(2)` clause.

4. Let's improve our timing. Basically, the very first kernel execution on GPU is always slow as the device needs to "wake up".
   Thus, when we are timing short executions like this one, the timings we see are misleading.
   In real codes, the "wake up time" is negligible in comparison to the total execution time.

   Modify the code so that it runs the whole calculation three times (including all memory transfers).
   Hint: you can do this conveniently in the `main()` function in the bottom of the source file.

   What kind of timings do you get now?

5. Increase the size of the system and compare the speed up to the serial CPU execution.
