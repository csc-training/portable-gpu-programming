# axpy operation with Kokkos parallel dispatch

The code [axpy.cpp](axpy.cpp) implements the basic "axpy" operation:
$y = a \times x + y$ in standard C/C++. Replace the `for` loops by
corresponding Kokkos parallel operation and the computational body by
functors. Make two versions, one with an explicit functor and one with a lambda function.

1. Build and run the code in LUMI with OpenMP backend. You can use your own Kokkos installation
or ready to use module
```
ml use /scratch/project_462001074/modulefile
ml kokkos/omp
```

2. Build and run the code in LUMI with HIP backend. You can use your own Kokkos installation
or ready to use module
```
ml use /scratch/project_462001074/modulefile
ml kokkos/hip
```
   - Can you explain why the code fails at run time?
   - In AMD systems one can control automatic data migration between host and device memories
     with `HSA_XNACK` environment variable
   - Try to set `export HSA_XNACK=1` before running the code, does it work now?

3. Build and run the code in Mahti with OpenMP backend. You can use your own Kokkos installation
or ready to use module
```
ml use /scratch/project_2015315/modulefiles
ml kokkos/omp
```

4. Build and run the code in Mahti with cuda backend. You can use your own Kokkos installation
or ready to use module
```
ml use /scratch/project_2015315/modulefiles
ml kokkos/cuda

   - NVIDIA A100 GPUs do not support automatic data migration for memory allocated with `malloc`.
   - In next lecture we look how to manage memory in portable way with Kokkos
```

