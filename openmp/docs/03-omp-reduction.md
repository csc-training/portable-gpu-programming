<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Reduction
event: Portable GPU Programming 2026
lang:  en
---

# Outline

- Reduction in OpenMP offload


# Reduction {.section}

# Reduction

- Reduction is a commonly needed operation
- OpenMP provides `reduction` clause that can perform
  the reduction efficiently without manual implementation
  and/or synchronization within the target region

# Reduction clause

**`reduction(operator:var1, var2, ...)`**

- Performs reduction on the (scalar) variables

This is what happens under the hood:

- A private reduction variable is created for each thread's partial result
- This private variable is initialized to the operator's initial value
- The compiler generates a reduction code to aggregate the partial results
  to the global variable

# Example

::::::{.columns}
:::{.column}
```c++
int n=1000;
double total=0.0;
double x[n*n], y[n*n];
// some code to initialise x and y

#pragma omp target map(tofrom: total)
#pragma omp teams distribute reduction(+:total)
for (int i = 0; i < n; i++) {
    #pragma omp parallel for reduction(+:total)
    for (int j = 0; j < n; j++) {
        total += x[i*n + j] * y[i*n + j];
    }
}
```
:::
:::{.column}
```fortranfree
integer :: n = 1000
real(8) :: total = 0.0
real(8), dimension(n, n) :: x, y
! some code to initialise x and y

!$omp target map(tofrom: total)
!$omp teams distribute reduction(+:total)
do j = 1, n
  !$omp parallel do reduction(+:total)
  do i = 1, n
      total = total + x(i,j) * y(i,j)
  end do
  !$omp end parallel do
end do
!$omp end teams distribute
!$omp end target
```
:::
::::::

- Note! The scalar `total` needs to be mapped tofrom the device!



# Reduction operators in C/C++ and Fortran

| Arithmetic Operator | Initial value |
| ------------------- | ------------- |
| `+`                 | `0`           |
| `-`                 | `0`           |
| `*`                 | `1`           |
| `max`               | least         |
| `min`               | largest       |


# Reduction operators in C/C++ only

<div class="column">
| Logical Operator | Initial value |
| ---------------- | ------------- |
| `&&`             | `1`           |
| `||`             | `0`           |
</div>

<div class="column">
| Bitwise Operator | Initial value |
| ---------------- | ------------- |
| `&`              | `~0`          |
| `|`              | `0`           |
| `^`              | `0`           |
</div>


# Reduction operators in Fortran only

<div class="column">
| Logical Operator | Initial value |
| ---------------- | ------------- |
| `.and.`          | `.true.`      |
| `.or.`           | `.false.`     |
| `.eqv.`          | `.true.`      |
| `.neqv.`         | `.false.`     |
</div>

<div class="column">
| Bitwise Operator | Initial value |
| ---------------- | ------------- |
| `iand`           | all bits on   |
| `ior`            | `0`           |
| `ieor`           | `0`           |
</div>





# Summary

- OpenMP provides reduction operation
- Remember to perform reduction at all levels of parallelism
