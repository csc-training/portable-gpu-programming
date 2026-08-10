<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Examining pointers

In this exercise we examine pointers for interoperability with external libraries.

The issue of calling external libraries boils down to the question how to get
the correct device pointer so that it can be passed to the library call.
OpenMP provides a clause for this: `use_device_ptr` / `use_device_addr`.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

## Tasks

1. The code `pointers` prints the address of an array
   in the host context and in the device context using `use_device_ptr` / `use_device_addr`.

   Compile and run the code on GPU and study the output.

2. Compile and run the code for CPU target and study the output.
