<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Solving 2D heat equation

In this exercise we practise the data mapping clauses across multiple GPU kernels.

The two-dimensional heat equation describes how temperature evolves over time:

$$
\frac{\partial u(x,y,t)}{\partial t} = \alpha \nabla^2 u(x,y,t)
$$

where $$u$$ is the temperature field to be solved and $$\alpha$$ is the thermal diffusivity.

Using finite differences, the heat equation can be discretized as

$$
\frac{u_{i,j}^{k+1}-u_{i,j}^{k}}{\Delta t} = \alpha \left( \frac{u_{i+1,j}^{k}-2u_{i,j}^{k}+u_{i-1,j}^{k}}{\Delta x^2} + \frac{u_{i,j+1}^{k}-2u_{i,j}^{k}+u_{i,j-1}^{k}}{\Delta y^2} \right)
$$

where $$u_{i,j}^{k}$$ is the temperature at grid point $$(i,j)$$ at time step $$k$$ and $$u_{i,j}^{k+1}$$ is the temperature at the next time step.

Rearranging terms gives the standard explicit finite-difference update formula

$$
u_{i,j}^{k+1} = u_{i,j}^{k} + \frac{\alpha \Delta t}{\Delta x^2} \left( u_{i+1,j}^{k}-2u_{i,j}^{k}+u_{i-1,j}^{k} \right) + \frac{\alpha \Delta t}{\Delta y^2} \left( u_{i,j+1}^{k}-2u_{i,j}^{k}+u_{i,j-1}^{k} \right).
$$

This discretized equation can be solved by advancing the solution in time.

To define the boundary conditions, we use fixed-temperature (Dirichlet) boundaries.
The temperature values at the edges of the domain are prescribed and remain constant throughout the simulation.
These boundary values are not updated by the finite-difference stencil.
Instead, the update formula is applied only to the interior grid points, while the boundary points retain their prescribed values at every time step.

The algorithm comprises of the following steps:

1. Initialize the grid with an initial temperature distribution $$u_{i,j}^{0}$$.
2. Iterate over all interior grid points and compute $$u^{k+1}$$ using values from $$u^{k}$$.
3. Replace $$u^{k}$$ with $$u^{k+1}$$.
4. Repeat for the desired number of time steps.

The provided code implements this algorithm in serial.

We have added timing using [`omp_get_wtime()`](https://www.openmp.org/spec-html/5.0/openmpsu160.html#x199-9660003.4.1),
so even the serial code needs to be compiled with `-fopenmp`.

You can provide input sizes as command line arguments: running the program for a 1024x1024 array, 500 time steps, and 1 repetition:

    ./heat.x 1024 500 1

Expected output:

    RUN 0
    Inputs: n = 1024, niter = 500
    Diffusivity: 0.50
    Box: 8.00 x 8.00 discretized with grid spacing 7.82e-03 x 7.82e-03
    Time propagation until 1.53e-02 with time step 3.06e-05
    u[511,511] = -2.522502
    Time spent: 0.490 s

We want to speed up this code by utilizing GPU!

The program also writes the temperature field to the disk: `u_initial.bin` and `u_final.bin`.
These files can be visualized on the [Roihu web interface](https://www.roihu.csc.fi/) by using the provided python script:

    module load python-data
    python3 heat-plot.py u_initial.bin

The [solution directory](solution/) contains a model solution and discussion on the exercises below.


## Tasks

1. Offload the stencil update to GPU by inserting a suitable OpenMP target directives around the loop (see 'TODO 1' in the code).
   Remember to include map clauses like in the previous exercise.

   Please ensure that the result remains correct.

   What is the speed up you get by using a single GPU over the serial CPU calculation?
   Or is the program actually slower?

   For timing, run the calculation for a few repetitions.
   Basically, the very first kernel execution on GPU is always slow as the device needs to "wake up".
   Thus, when we are timing short executions like this one, the timings we see are misleading.
   In real codes, the "wake up time" would be negligible in comparison to the total execution time.

2. Enable runtime debugging and examine the memory transfers during the execution.

3. Insert separate data transfer clauses around the time propagation loop to ensure that there is no unnecessary copying back and forth between
   the CPU and GPU memories (see 'TODO 2' in the code).

   What is the speed up you get now?

4. Could you write the kernel launch pragmas (`target teams distribute parallel for`) in some other ways around the two loops?
   What kind of speed ups do you observe? Hint: try also `collapse(2)` clause.

5. Increase the size of the system and compare the speed up to the serial CPU execution.

6. (Bonus) Use `nsys` (Roihu) or `rocprof` (LUMI) profiler to analyze the code and visualize its execution timeline.
