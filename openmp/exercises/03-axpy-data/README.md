<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Data regions in the axpy code

In this exercise we start looking on data mapping clauses.

We developing further the axpy code from the previous exercise,
the model solution of which is provided here as a starting code.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

## Tasks

1. Offload also the array initialization loop to GPU and create a single structured
   data region covering both GPU kernels.

   Enable runtime debugging to confirm that there is no unnecessary data movements between
   the host and the device between the initialization and calculation loops.

   Hint: use `target update` within the data region to ensure that correct arrays are printed on CPU
   after the initialization.

2. For practice, replace the structured data region with an unstructured data region.

