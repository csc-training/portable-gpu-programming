<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title:  Data parallel patterns in Kokkos
subtitle: Portable GPU Programming
date: October 2025
lang:     en
---

# Learning objectives

- How to program Kokkos
- How to express data parallel patterns
- Parallelizing simple loops and and performing reductions

# Programming Kokkos

- Kokkos (like SYCL) utilizes heavily modern C++ features like lambdas/functors and templates
- The Kokkos API is accessed through `Kokkos_Core.hpp` header file
- Kokkos code starts with Kokkos initialization and ends with finalization
<small>
```
Kokkos::initialize(argc, argv);
{
...
}
Kokkos::finalize();
```
</small>

- Initialization parameters can be passed via `--kokkos-xxxx` command line arguments

# Expressing parallelism

- Data parallel patterns contain three concepts
- Computational body, *.i.e.* unit of work
- Iteration range which defines the total amount of work
- Mapping of the work to execution resources

```c++
for (int i=0; i < N; i++) // Iteration range
{
  y[i] += a * x[i]; // computational body
}
```

# Expressing parallelism in Kokkos

- When using Kokkos, computational body is defined as functor (explicit or lambda)
- Mapping of work is carried by the parallel dispatch operations of Kokkos
- Parallel dispatch operations take as an argument the iteration range and the functor
- Functor is called at each iteration step with the iteration index
- Iteration steps may be done in parallel, with no guarantee about the order they are executed

# Parallel axpy with Kokkos

<div class="column">
Using an explicit functor
```c++
class axpy {
  const double *_x, 
  double *_y;
  const double _a;
public:
  axpy(double *x, double *y, double a) :
    _x(x), _y(y), _a(a) {};
  void operator()(const int i) const {
    _y[i] += _a * _x[i];
  }
}

Kokkos::parallel_for(N, axpy(x, y, a));
```
</div>

<div class="column">
Using a lambda expression
```c++
Kokkos::parallel_for(N,
   [=] (const int i) {
        y[i] += a * x[i];
   });
```
</div>

# Portability macros for functors

- Compiling functions for CUDA and HIP requires special annotations (`__device__`)
- In order to ensure single source both for host and device backends, Kokkos provides special 
  portability macros
- `KOKKOS_INLINE_FUNCTION` for functions (e.g. functor operators)
- `KOKKOS_LAMBDA` for lambdas
- `KOKKOS_CLASS_LAMBDA` for lambdas inside class member functions

# Parallel axpy with portability macros

<div class="column">
Using an explicit functor
```c++
class axpy {
  const double *_x, 
  double *_y;
  const double _a;
public:
  axpy(double *x, double *y, double a) :
    _x(x), _y(y), _a(a) {};
  KOKKOS_INLINE_FUNCTION
  void operator()(const int i) const {
    _y[i] += _a * _x[i];
  }
}

Kokkos::parallel_for(N, axpy(x, y, a));
```
</div>

<div class="column">
Using a lambda expression
```c++
Kokkos::parallel_for(N,
   KOKKOS_LAMBDA (const int i) {
     y[i] += a * x[i];
   });
```
</div>

# Reduction

- Consider dot product $x \cdot y = \sum_i x[i] * y[i]$
- Serial code
```c++
result = 0.0;
for (size_t i = 0; i < N; i++)
{
  result += x[i] * y[i];
}
```

# Reduction

- Naive parallel implementation
```c++
result = 0.0;

Kokkos::parallel_for(N,
   KOKKOS_LAMBDA (const int i) {
     result += x[i] * y[i];
   });
```

# Reduction

- Naive parallel implementation
```c++
result = 0.0;

Kokkos::parallel_for(N,
   KOKKOS_LAMBDA (const int i) {
     result += x[i] * y[i];
   });
```
- Two problems:
    - Lambda capture by value, update not allowed
    - Race condition, multiple threads might modify result at the same time

# Reduction

- Reduction is a common parallel pattern where results of parallel work are combined
- Kokkos provides `parallel_reduce` operation
```c++
  double result = 0.0;
  Kokkos::parallel_reduce(N,
    KOKKOS_LAMBDA (const size_t i, double &local_result) {
    local_result += x[i] * y[i];
  }, result);
```
- Default reduction operator is sum, by replacing `result` with a "Reducer" other reductions can be carried out 

# Parallel dispatch operators in Kokkos

- `parallel_for`, `parallel_reduce`, and `parallel_scan` (prefix sum)
- Parallel operation may be executed asynchronously
    - `Kokkos::fence()` can be used for synchronization
- Operators have an optional "name" argument
```c++
  parallel_for("axpy", N, ...)
```
- "name" is used by profiling and debugging tools

# Summary

- In Kokkos, computational work is defined in functors / lambdas
- Parallel dispatch operations map work into execution resources
    - `parallel_for`, `parallel_reduce`, `parallel_scan`
- Computational work may be executed in parallel, in any order, and asynchronously
