## Using unified shared memory

In this exercise we practise the use of unified shared memory in OpenMP.

We try axpy and Jacobi codes from previous exercises with unified shared memory.

The sample codes here are the latest model codes for both cases, with
`#pragma omp requires unified_shared_memory` inserted at the top of the files.

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

### Exercises for axpy

1. Try compiling and running the axpy code.

   It's expected to see the error

       ACC: CRAY_ACC_ERROR - OpenMP program 'requires unified_shared_memory', but the current device does not support unified shared memory

   This is because page migration between GPU and CPU memories is disabled by default on LUMI.
   To enable it in run time, set

       export HSA_XNACK=1

   Try then running the code. Set also `CRAY_ACC_DEBUG=2` and compare the output to previous cases.

   Remove all data mapping statments from the USM version of the code. Does it still run?
   What happened with the previous version of the code with out USM?

### Exercises for Jacobi

1. Try compiling and running the code.

   How does the runtime compare to previous cases?

   Do you get correct results and correct output files?
