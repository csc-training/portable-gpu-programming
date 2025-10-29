<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Further material
event:  Portable GPU Programming 2025
lang:   en
---

# Outline

- OpenMP API functions
- Declaring device functions

# OpenMP API functions {.section}

# Runtime API functions

- Low-level runtime API functions can be used to
  - Query the number of devices in the system
  - Select the device to use
  - Allocate/deallocate memory on the device(s)
  - Transfer data to/from the device(s)
- Function definitions are in
  - C/C++ header file `omp.h`
  - `omp_lib` Fortran module


# Useful API functions

`omp_is_initial_device()`
  : returns True when called in host, False otherwise

`omp_get_num_devices()`
  : number of devices available

`omp_get_device_num()`
  : number of device where the function is called

`omp_get_default_device()`
  : default device

`omp_set_default_device(n)`
  : set the default device

# Declaring device functions {.section}

# Function calls in compute regions

- Often it can be useful to call functions within loops to improve readability and modularisation
- By default OpenMP does not create accelerated regions for loops calling functions
- One has to instruct the compiler to compile a device version of the function

# Directive: `declare target`

- Define a function to be compiled for an accelerator as well as the host
- C/C++
  - Enclose function declaration within `#pragma omp declare target` 
    and `#pragma omp end declare target`
- Fortran
  - Use `!$omp declare target` within the subroutine
- The functions will now be compiled both for host and device execution

# Example: `declare target`

<div class="column">
**C/C++**
```c
#pragma omp declare target
void foo(float* v, int i, int n) {
    for ( int j=0; j<n; ++j) {
        v[i*n+j] = 1.0f/(i*j);
    }
}
#pragma omp end declare target

#pragma omp target teams loop
for (int i=0; i<n; ++i) {
    foo(v,i);  // executed on the device
}
```
</div>

<div class="column">
**Fortran**
```fortranfree
subroutine foo(v, i, n)
  !$omp declare target
  real :: v(:,:)
  integer :: i, n

  do j=1,n
     v(i,j,n) = 1.0/(i*j)
  enddo
end subroutine

!$omp target teams loop
do i=1,n
  call foo(v,i,n)
enddo
!$omp end target teams loop
```
</div>


# Declare variables

- Makes a variable resident in accelerator memory
  - C/C++: `#pragma omp declare target (list)`
  - Fortran: `!$omp declare target (list)`
- Added after the declaration of a variable
- Data life-time on device is the implicit life-time of the variable


# Summary

- OpenMP provides many API functions
  - See specification and examples in <https://www.openmp.org/specifications/>
- Declare target directive enables one to write device functions that can be called within target regions
