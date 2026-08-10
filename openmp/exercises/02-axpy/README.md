<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: First GPU kernel: axpy

In this exercise we practise parallelizing an axpy code using OpenMP offload.

The axpy operation is a fundamental linear algebra operation defined as

$$
y_i \leftarrow \alpha x_i + y_i
$$

where $\alpha$ is a scalar and $x$ and $y$ are vectors of the same size.

A serial example code is provided: The code initializes the input values $\alpha$, $x$, and $y$,
performs axpy operation, and prints the output $y$.

Expected output of the provided code:

    Array size n = 102400
    Input:
    a =   3.0000
    x =   0.0000   0.0000   0.0000   0.0000 ...   1.0000   1.0000   1.0000   1.0000
    y =   0.0000   0.0010   0.0020   0.0029 ...  99.9971  99.9980  99.9990 100.0000
    Output:
    y =   0.0000   0.0010   0.0020   0.0030 ... 102.9970 102.9980 102.9990 103.0000

The [solution directory](solution/) contains a model solution and discussion on the tasks below.

## Tasks

1. Study, compile, and run the provided code. You can provide the array size as a command line argument, e.g., `./axpy.x 1024`.

   Note: the code includes a separate `axpy_helper_functions.{h,F90}` file that provides the `print_array()`
   helper function. You don't need to study the contents of the helper file, but you can use it as such.

2. Offload the axpy loop to GPU by adding suitable OpenMP directives (see 'TODO' in the code).

   Note! Remember to add also suitable data mapping clauses.

3. Would the program work without data mapping clauses?

   Hint: enable runtime debugging and examine the data transfers done with and without data mapping clauses.

4. (Bonus) Enable compiler diagnostics and study the compiler output.


### Bonus tasks: Offload to CPU threads

1. Load the modules for CPU execution.

2. Compile your OpenMP-offload code to CPU threads.

   Cray compiler wrappers choose the offload target based on the loaded modules that set specific environment variables,
   so now with these modules loaded, the same compilation command will target CPU threads.

3. Run the program on a CPU partition with, e.g., 4 threads.
   Set an environment variable to display thread affinities:

       export OMP_DISPLAY_AFFINITY=true

   Is the code executing correctly using threads?
