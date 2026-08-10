<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Using unified shared memory

In this exercise we practise the use of unified shared memory in OpenMP.

We try the axpy code from previous exercises with unified shared memory.

The sample codes here are the latest model codes for both cases, with
`omp requires unified_shared_memory` inserted at the top of the files.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

## Tasks

1. Try compiling and running the axpy code.

   On LUMI, it's expected to see the error

       ACC: CRAY_ACC_ERROR - OpenMP program 'requires unified_shared_memory', but the current device does not support unified shared memory

   This is because page migration between GPU and CPU memories is disabled by default on LUMI.
   To enable it in run time, set

       export HSA_XNACK=1

   Remove all data mapping statments from the USM version of the code. Does it still run?
   What happened with the previous version of the code without USM?
