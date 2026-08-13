<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Heat equation: Using multiple GPUs

In this exercise we parallelizing the heat equation over multiple GPUs.

For simplicity, the starting point is the heat equation code from the first exercise,
with only the time-evolution kernel.

The code has MPI parallelization implemented for scattering the initial
data across MPI tasks as well as for gathering it after the calculation
has finished.

However, the critical parts are missing and the code is not working properly.

The [solution directory](solution/) contains a model solution and discussion on the tasks below.

## Tasks

1. Study, compile, and run the code with multiple MPI tasks and GPUs.

   Visualize the final temperature field. Does it look odd?

2. Set different GPU device for each MPI rank and
   implement halo exchange between GPUs (see TODOs in the code).

   Do you get correct results now?

   Does the simulation get faster with multiple GPUs?
