<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Heat equation: Monitoring average temperature

In this exercise we practise reduction operations.

We extend the heat equation code from the previous exercise to monitor the average
temperature across the four quadrants of the grid every 100th iteration.

The current code attempts to calculate the averages on the CPU, but since the data
resides on the GPU, it is reading stale data as
the average is computed from the CPU copy which is not up to date.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

## Tasks

1. Fix the calculation of the average by offloading it to GPU and by using the reduction clause.

   Is the output expected?

   Does computing the average on every 100th step noticeably impact performance?
