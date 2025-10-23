## Solving 2D Poisson's equation with Jacobi iteration

In this exercise we port 2D Poisson equation solver to Kokkos

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


The existing serial non-Kokkos code can be build with CMake, or just by

    cc -O3 -fopenmp poisson.c -o poisson

Run the program on a single CPU core for a an 1024x1024 array for 500 iterations:

    srun -p debug --nodes=1 --ntasks-per-node=1 --cpus-per-task=1 -t 0:10:00 ./poisson 1024 500

Expected output:

    Using n = 1024, niter = 500
    u[512,512] = -120.808781
    Mean u = -0.041238
    Time spent: 12.614 s



### Exercise: port the code to Kokkos

1. Replace appropriate data structures with Kokkos Views
   
2. Use parallel dispatch for all the loops (initialization, computation, error checking)

3. Copy data between device and host when necessary   

4. Build and run the code both with OpenMP and HIP/CUDA backend. Please ensure that the result remains 
   same as with the serial non Kokkos version. Run the OpenMP version with different number of
   CPU cores / threads, does the performance improve when using more threads?

5. Compare the performance to serial version as well as the difference between OpenMP and GPU versions. 

6. Try to specify the layout explicitly as LayoutLeft and LayoutRight, and investigate how that affects performance both on OpenMP and device (CUDA/HIP)
