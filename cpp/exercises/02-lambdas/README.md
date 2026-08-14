<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Using lambdas

In this exercise you can practice implementing simple operations with lambda functions.
In this case lambdas do not provide added value, we are just trying to illustrate the concept.
in parallel using Kokkos. In order to focus only on
parallel dispatch, the proper way to manage memory with Kokkos is not considered yet.

## Tasks

The code [dot-product.cpp](dot-product.cpp) implements dot product $x \cdot y = \sum_i x[i] * y[i]$
in standard C/C++.

The code has two `for` loops, the first for initializing the input data and the second for evaluating
the dot product. Create lambda functions for both loop bodies outside the loops, and call the
lambda function within the loop. Try different capture clauses (by value and by reference), do you
see difference in the behaviour of the code?
