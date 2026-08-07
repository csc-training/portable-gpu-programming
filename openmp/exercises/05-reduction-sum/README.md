<!--
SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Parallel sum

In this exercise we practise OpenMP reduction construct.

The provided example code attempts to calculate in parallel the sum

$$
\sum_{i = 0}^{N} \sin(i)
$$

However, there is a race condition in the code as executing the code
multiple times results in different outputs.

The [solution directory](solution/) contains a model solution and discussion on the tasks below.


## Task: Fix the race condition

1. Study, compile, and run the provided code.
   Run the code multiple times to observe the race condition.

2. Use the `reduction` clause to fix the race condition.
