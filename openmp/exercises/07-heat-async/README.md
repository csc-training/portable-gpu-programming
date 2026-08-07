<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Heat equation: Asynchronous kernel execution

In this exercise we practise asynchronous kernel execution.

We start from the previous heat code.

The kernel launches are synchronous by default in OpenMP offload.
This means that the host process is waiting for the GPU kernel to finish before it proceeds with the host code.
This doesn't allow keeping the GPU fully busy, so let's fix this by using asynchronous execution.

The [solution directory](solution/) contains a model solution and discussion on the tasks below.

## Tasks

1. Use profiler to study the current code behavior. Pay attention to GPU and CPU execution and possible gaps between kernels.

2. Based on the knowledge learned from the profile, improve the code by adding `nowait` clauses
   with suitable `depend` clauses in the kernel to make the launch asynchronous while keeping the results correct.
