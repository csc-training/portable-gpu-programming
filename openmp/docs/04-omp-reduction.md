<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Reduction
event:  Portable GPU Programming 2025
lang:   en
---

# Outline

- Reduction in OpenMP offload


# Reduction {.section}

# Reduction

- Reduction is a commonly needed operation
- OpenMP provides `reduction` clause that can perform
  the reduction efficiently without manual implementation
  and/or synchronization within the target region

# Reduction

`reduction(operation:list)`
  : `-`{.ghost}

- Applies the *operation* on the variables in *list* to reduce them to a single value
  - Local private copies of the variables are created for each thread
  - Initialisation depends on the *operation*
- Variables need to be shared in the enclosing parallel region
  - At the end, all local copies are reduced and combined with the original shared variable


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


# Example

<div class=column>
```c++
int n=1000;
double sum=0.0;
double x[n*n], y[n*n];
// some code to initialise x and y

#pragma omp target map(tofrom: sum)
#pragma omp teams distribute reduction(+:sum)
for (int i = 0; i < n; i++) {
    #pragma omp parallel for reduction(+:sum)
    for (int j = 0; j < n; j++) {
        sum += x[i*n + j] * y[i*n + j];
    }
}
```
</div>
<div class=column>
```fortranfree
integer :: n = 1000
real(8) :: sum = 0.0
real(8), dimension(n, n) :: x, y
! some code to initialise x and y

!$omp target map(tofrom: sum)
!$omp teams distribute reduction(+:sum)
do j = 1, n
  !$omp parallel do reduction(+:sum)
  do i = 1, n
      sum = sum + x(i,j) * y(i,j)
  end do
  !$omp end parallel do
end do
!$omp end teams distribute
!$omp end target
```
</div>




# Summary

- OpenMP provides reduction operation
- Remember to perform reduction at all levels of parallelism
