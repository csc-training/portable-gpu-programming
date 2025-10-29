<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Interoperability with libraries
event:  Portable GPU Programming 2025
lang:   en
---

# Outline

- Using accelerated libraries from OpenMP offload

# Using accelerated libraries from OpenMP offload {.section}

# Interoperability with libraries

- Often it may be useful to integrate the accelerated OpenMP code with other accelerated libraries
- MPI: GPU-aware MPI libraries can do GPU-to-GPU memory transfer without going through host
- CUDA/HIP: It is possible to mix OpenMP and CUDA/HIP
  - Use OpenMP for memory management
  - Introduce OpenMP in existing GPU code
  - Use CUDA/HIP for tightest kernels, otherwise OpenMP
- Numerical GPU libraries: cublas/hipblas, cufft/hipfft, ...

# Device data interoperability

- OpenMP includes methods to access the device data pointers on the host side
- Device data pointers can be used to interoperate with libraries and
  other programming techniques available for accelerator devices

# Data constructs: `use_device_ptr` and `use_device_addr`

**`omp target data use_device_ptr(var-list)`**

- Within the construct, all the pointer variables in `var-list` correspond to the device addresses
- Use in C for pointer or in Fortran for `c_ptr`

**`omp target data use_device_addr(var-list)`**

- Similar construct for non-pointer variables
- Use for arrays in Fortran

- See detailed description in the OpenMP standard


# Example: MPI

<div class=column>
```c++
double *data;
...

#pragma omp target data map(to:data[0:N])
{
    ...
    #pragma omp target data use_device_ptr(data)
    {
        MPI_Send(data, N, MPI_DOUBLE,
                 1, 123, MPI_COMM_WORLD);
    }
...
}
```
</div>

<div class=column>
```fortranfree
real(8), allocatable :: data(:)
...

!$omp target data map(to: data(1:N))
...

!$omp target data use_device_addr(data)

call MPI_Send(data, N, MPI_DOUBLE_PRECISION, &
              1, 123, MPI_COMM_WORLD, ierr)
!$omp end target data
...
!$omp end target data
```
</div>

- This code uses GPU-aware MPI, no GPU-to-CPU memory copy needed for MPI

# Example: hipblas

<div class=column>
C:
```c
#pragma omp target data map(to: x[0:n]) \
                        map(tofrom: y[0:n])
{
    #pragma omp target data use_device_ptr(x, y)
    {
        hipblasDaxpy(handle, n, &alpha,
                     x, 1, y, 1);
    }
}
```
</div>

<div class=column>
Fortran:

- The `iso_c_binding` module can be used to build a Fortran interface to such C library

- See exercises
</div>

# Summary

- OpenMP programs can work in conjuction with GPU libraries or with own computational kernels written in other languages
- The pointer to the data in device can be obtained with the `use_device_ptr` construct
