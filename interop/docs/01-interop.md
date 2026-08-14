<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Interoperability with CUDA/HIP and libraries
event: Portable GPU Programming 2026
lang:  en
---

# Outline

- Using CUDA/HIP kernels and accelerated libraries together with OpenMP offload and Kokkos

# Interoperability with CUDA/HIP and libraries {.section}

# Interoperability with CUDA/HIP and libraries

- OpenMP offload and Kokkos codes can be integrated with CUDA/HIP kernels and libraries
- Example use cases for mixing OpenMP/Kokkos with CUDA/HIP
  - Use OpenMP/Kokkos for memory management
  - Introduce OpenMP/Kokkos in existing GPU code
  - Use CUDA/HIP for most critical kernels, otherwise OpenMP/Kokkos
- Calling GPU libraries: cublas/hipblas, cufft/hipfft, ...
- Using MPI: GPU-aware MPI libraries can do GPU-to-GPU memory transfer without going through host

# Device data interoperability

- Device data pointers can be used to interoperate with libraries and CUDA/HIP
- OpenMP and Kokkos have dedicated methods to
  1. Obtain the device data pointers for the data managed by them
  2. Use externally managed data


# Interoperability in OpenMP offload {.section}

# Passing data from OpenMP

- **`target data use_device_ptr(var1, var2, ...)`**
  - Within the construct, all the listed pointer variables correspond to the device addresses
  - Use in C for pointers or in Fortran for `c_ptr`

- **`target data use_device_addr(var1, var2, ...)`**
  - Similar construct for non-pointer variables
  - Use for arrays in Fortran

- See detailed description in the OpenMP standard

# Example: Calling CUDA/HIP kernel

```cpp
#pragma omp target data map(to: x[0:n]) \
                        map(tofrom: y[0:n])
{
  #pragma omp target data use_device_ptr(x, y)
  {
    // Execute kernel on device
    axpy<<<griddim, blockdim>>(alpha, x, y, n)
  }
  // Wait for device to finish
  hipDeviceSynchronize();
}
// Calculated y is now available on host
...

```

- The `iso_c_binding` module can be used to build a Fortran interface to such C library


# Example: Calling hipblas

```cpp
#pragma omp target data map(to: x[0:n]) \
                        map(tofrom: y[0:n])
{
  #pragma omp target data use_device_ptr(x, y)
  {
    // Execute kernel on device
    hipblasDaxpy(handle, n, &alpha, x, 1, y, 1);
  }
  // Wait for device to finish
  hipDeviceSynchronize();
}
// Calculated y is now available on host
...
```

- The `iso_c_binding` module can be used to build a Fortran interface to such C library


# Alternative for `use_device_ptr`: `omp_get_mapped_ptr`

- If host pointer `x` has been mapped to device with OpenMP,
  we can get the corresponding device pointer `d_x` by
  ```cpp
  void* d_x = omp_get_mapped_ptr(x, omp_get_device_num())
  ```

# Using externally managed data in OpenMP

- If `d_x` is already a device pointer (e.g., allocated outside OpenMP),
  we can instruct OpenMP to use that pointer instead doing mapping:
  ```cpp
  #pragma omp target ... is_device_ptr(d_x)
  ```

# Interoperability with Kokkos {.section}

# Passing data from Kokkos

- The raw pointer to the data in the Kokkos View is obtained with the `data()` method

  ```cpp
  Kokkos::View<float**> a("a", n, m);
  auto a_ptr = a.data();

  some_external_function(a_ptr, n, m, ...)
  ```

- Memory layout may need special considerations

# Combining Kokkos with CUDA/HIP

- Kokkos allow to include e.g. CUDA/HIP code even in the same source with the Kokkos code
- Might be used for special low level optimization

  ```cpp
  Kokkos::View<float**> a("a", n, m);
  ...
  #ifdef KOKKOS_ENABLE_CUDA

  __global__ void my_cuda_kernel(float *a, int n, int m) {};

  // CUDA grid and block need to be defined
  my_cuda_kernel<<<grid, block>>>(a.data(), n, m);
  #else
  parallel_for(...);
  #endif
  ```

# Using externally managed data in Kokkos

- Kokkos allows to wrap data allocated outsize Kokkos inside an Unmanaged View
    - No reference counting (and automatic deallocation)
    - No check for correct memory space
- Unmanaged View can be created by passing a raw pointer as first argument to View
    - Memory space, dimensions and layout need to be also specified
    - No label

```cpp
Kokkos::View<double**, Kokkos::LayoutRight,
             Kokkos::HostSpace, Kokkos::MemoryUnmanaged> a(a_ptr, n, m);
```


# Summary {.section}
# Summary

- OpenMP and Kokkos programs can work in conjuction with GPU libraries or with own computational kernels written in other languages
- In many cases, only thing needed is to obtain the raw pointer to the data

