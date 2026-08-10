<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Device functions and variables
event: Portable GPU Programming 2026
lang:  en
---

# Outline

- Declaring device functions and variables

# Declaring device functions and variables {.section}

# Function calls in compute regions

- Often it can be useful to call functions within loops to improve readability and modularisation
- By default OpenMP does not create accelerated regions for loops calling functions
- The `declare target` tells the compiler that certain functions or variables should be compiled for the device (not just the host CPU)
  - Analogous to `__device__` in CUDA/HIP

# Example: Device functions in C

::::::{.columns}
:::{.column}
`main.c`:

```cpp
#pragma omp target
#pragma omp teams distribute parallel for
for (int i = 0; i < n; i++) {
  z[i] = fun(x[i], y[i]);
}
```
:::
:::{.column}
`kernels.c`:

```cpp
#pragma omp declare target
double fun(double x, double y) {
  return x + y;
}
#pragma omp end declare target
```
:::
::::::

# Example: Device functions in Fortran

::::::{.columns}
:::{.column}
`main.F90`:

```fortranfree
!$omp target
!$omp teams distribute parallel do
do i = 1, n
  z(i) = fun(x(i), y(i))
end do
!$omp end teams distribute parallel do
!$omp end target
```
:::
:::{.column}
`kernels.F90`:

```fortranfree
module kernels
  implicit none
contains

  pure function fun(x, y) result(res)
    real(8), intent(in) :: x, y
    real(8) :: res
    !$omp declare target

    res = x + y
  end function fun

end module kernels
```
:::
::::::


# Declare variables

- Makes a variable resident in accelerator memory
  - C/C++: `#pragma omp declare target (list)`
  - Fortran: `!$omp declare target (list)`
- Added after the declaration of a variable
- Data life-time on device is the implicit life-time of the variable


# Summary {.section}
# Summary

- Declare target directive enables one to write device functions that can be called within target regions

