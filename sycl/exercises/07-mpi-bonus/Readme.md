<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# MPI send and receive with GPUs

Here are example codes with MPI + {SYCL} for two MPI tasks.
Task 0 fills an array with values and sends the array to task 1 that receives the values.
Finally both tasks print their arrays.

These codes are minimal examples for associating MPI tasks with available GPUs and performing MPI communication using GPU-aware MPI.
