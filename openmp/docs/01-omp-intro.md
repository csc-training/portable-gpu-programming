<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Introduction to OpenMP offload
event: Portable GPU Programming 2026
lang:  en
---

# Outline

- What is OpenMP?
- OpenMP offload in practice: Creating, compiling, and running a GPU-accelerated program
- Three components of OpenMP: directives, runtime library functions, and environment variables

# What is OpenMP? {.section}

# What is OpenMP?

- OpenMP is an application programming interface (API) for creating parallel programs in C, C++, and Fortran using **shared-memory multithreading** and/or **accelerator offloading** (GPUs)
- OpenMP is a standard
  - First version (1.0) published in 1997
  - Version 4.0 introduced OpenMP offload in 2013
  - Latest version (6.0) published in 2024
  - <https://www.openmp.org/specifications/>
  - See especially the official OpenMP examples documents!

# Compilers implement OpenMP

- The OpenMP standard is implemented by different compilers
  - [GCC](https://gcc.gnu.org/) 15 supports all of OpenMP 4.5, most of 5.0, 5.1, and 5.2, and the first 6.0 features for C, C++, and Fortran
  - [Clang](https://www.llvm.org/) 21 supports all of OpenMP 4.5, almost all of 5.0 and most of 5.1 and 5.2 features for C and C++
  - [NVIDIA HPC](https://developer.nvidia.com/hpc-sdk) 26.3 supports all of OpenMP 3.1 and a subset of OpenMP 5.1 in C, C++, and Fortran
  - See [this list](https://www.openmp.org/resources/openmp-compilers-tools/) for more


# The idea of OpenMP offload

- Write the code as if it was serial and then annotate code regions with **compiler directives** that instruct the compiler
  to parallelize and offload the regions to the GPU
  - If compiled without OpenMP, then directives are ignored and code is a regular serial code
- The same code can be compiled and run on various systems
  - NVIDIA GPUs, AMD GPUs, Intel GPUs, FPGAs, ...
  - CPUs only (threading, vectorization)

# Directive languages and performance

- "Write once, run everywhere"
  - It is true that you can get portability
  - It is *not necessarily* true that you get *performance* portability

- It is possible to optimize a code for performance on the GPU
  - Many optimisations will likely increase the performance also on the CPU
  - A highly GPU-optimized code will possibly be slower on the CPU

# OpenMP vs. OpenACC

- OpenACC is similar compiler directive based approach for GPU programming
  - Open standard, NVIDIA major driver
- Why OpenMP and not OpenACC?
  - OpenMP is likely to have a more extensive platform and compiler support
  - Currently, OpenACC support in AMD GPUs is limited (supported only for Fortran on LUMI),
    but OpenACC can provide better performance on NVIDIA GPUs

# OpenMP vs. CUDA/HIP

- Why OpenMP and not CUDA/HIP?
  - Less code to start utilizing GPU acceleration
  - Simple things are possibly simpler
  - Same code can be compiled for CPU and GPU targets
- Why CUDA/HIP and not OpenMP?
  - Can access all the features of the GPU hardware
  - Better control and assurance that the code will work as intended
  - More optimization possibilities
- Note! It's possible to use **both** OpenMP and CUDA/HIP in an application
  - Example: the bulk of the code accelerated with OpenMP,
    performance-critical parts optimized with CUDA/HIP


# OpenMP offload in practice {.section}

# Execution model

- Host-directed execution with an attached accelerator
  - Program runs on the host (CPU)
  - Host offloads compute-intensive regions (*kernels*)
  - GPU executes the kernels

![](img/gpu-offload.svg){.center width=40%}

# Data model

- If host memory is separate from device memory
  - Host manages memory of the device
  - Host copies data to/from the device
- When memories are not separate, no copies are needed (difference is transparent to the user)
  - With unified shared memory, physically separate memories can also be used
    as if they were the same memory (copies take place under the hood nevertheless)


# Programming OpenMP

- OpenMP is mainly programmed through compiler directives, i.e., language extensions
  - Also runtime library routines exist
- OpenMP directives start with `#pragma omp` / `!$omp`, followed by the directive name (e.g., `target`) and optional clauses

<div class=column>
```c++
#pragma omp target [clauses]
{
  // This block is executed in device
}
```
</div>

<div class=column>
```fortranfree
!$omp target [clauses]

  ! This block is executed in device
!$omp end parallel
```
</div>

- Directives are ignored when the code is compiled without OpenMP support $\to$ regular serial code

# Compiling an OpenMP offload program

- OpenMP is enabled during compilation with a command line option
  - GNU, Clang, Intel, Cray: `-fopenmp`
  - Intel legacy: `-qopenmp`
  - NVIDIA: `-mp`
- In addition, one needs to typically specify offload target (NVIDIA GPU, AMD GPU, ...)
- Compiler can also provide diagnostics
  - If and how loops were parallelized, what kind of data copies were added, ...
- See exercises

# Running an OpenMP offload program

- The compiled code can be executed as usual on supercomputer
- OpenMP implementations provide useful environment variables that can provide useful debug information in runtime
  - `NVCOMPILER_ACC_NOTIFY`, `CRAY_ACC_DEBUG`, `LIBOMPTARGET_INFO`, ...
- Many environment variables exists for controlling the execution
- See exercises


# Three components of OpenMP {.section}

# Three components of OpenMP

- Compiler directives
- Runtime library routines
- Environment variables

# OpenMP constructs for GPU programming

- Executing on device
  - Offload a code region to a device: `target`
  - Create teams and threads on the device: `teams`, `parallel`
  - Distribute work across the created teams and threads: `distribute`, `for`/`do`
- Controlling data movement
  - Transferring data in a single offloaded region only: `target map`
  - Transferring data in larger regions: `target data map`, `target enter data map`, `target exit data map`

# OpenMP constructs for executing on device {.section}

# Target construct

- OpenMP `target` construct specifies a region to be executed on GPU
  - Initially, runs with a single thread
- By default, execution in the host continues only after target region is finished
  - Asynchronous execution is also possible
- May trigger implicit data movements between the host and the device

<div class=column>
```c++
#pragma omp target
{
  // This block is executed in device
}
```
</div>

<div class=column>
```fortranfree
!$omp target

  ! This block is executed in device
!$omp end target
```
</div>

# Constructs that *generate parallelism* vs *distribute work*

- Target construct does not create any parallelism, so additional constructs are needed
- OpenMP `teams` and `parallel` constructs *generate parallelism*
  - `teams` and `parallel` create multiple teams and threads (that do the same computation by default)
- OpenMP `distribute` and `for`/`do` constructs *distribute work* so that the created teams and threads do different work
  - `distribute` and `for`/`do` assign different teams and threads to different loop iterations (so that the computation work is distributed)

# Creating teams

- A `teams` construct creates a *league of teams*
  - Number of teams is implementation-dependent
  - Initially, there is only a single thread in each team
- No synchronization between teams is possible
- *Likely mapping* to hardware
  - Team &rarr; thread block, runs on a streaming multiprocessor (SM) / on a compute unit (CU)
  - Note! This mapping is not enforced by OpenMP standard; implentation has freedom to decide


# Creating threads within a team

- Just having a league of teams is typically not enough to leverage all the parallelism available in the accelerator
- A `parallel` construct within a `teams` region creates threads within each team
  - Number of threads per team is implementation-dependent
  - With N teams and M threads per team there will be N x M threads in total
- Threads within a team can synchronize
- *Likely mapping* to CUDA/HIP and hardware
  - Thread &rarr; thread, runs on a core of SM / on a lane of a SIMD unit of CU
  - Note! This mapping is not enforced by OpenMP standard; implentation has freedom to decide


# Creating teams and threads

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams
#pragma omp parallel
{
  // This block is executed in device
}
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams
!$omp parallel
  ! This block is executed in device
!$omp end parallel
!$omp end teams
!$omp end target
```
:::
::::::

- Useful API functions: `omp_get_team_num()`, `omp_get_thread_num()`, `omp_get_num_teams()`, `omp_get_num_threads()`

- Demo: `hello`


# Distributing work across teams and threads

- The `teams` and `parallel` constructs create teams and threads, however, all the threads are still executing the same code
- The `distribute` construct distributes loop iterations over teams
- The `for` / `do` construct distributes loop iterations over threads
- Note! The `distribute` and `for` / `do` assume that the loop iterations are independent
  - Compiler does not check if that is the case; it's the programmer's responsibility


# Distributing work across teams and threads

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams
#pragma omp distribute
for (int i = 0; i < N; i++) {
  #pragma omp parallel
  #pragma omp for
  for (int j = 0; j < M; j++) {
    ...
  }
}
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams
!$omp distribute
do i = 1, N
  !$omp parallel
  !$omp do
  do j = 1, M
    ...
  end do
  !$omp end do
  !$omp end parallel
end do
!$omp end distribute
!$omp end teams
!$omp end target
```
:::
::::::

- Demo: `hello`

# Controlling the number of teams and threads

- By default, the number of teams and the number of threads is up to the implementation to decide
- The `num_teams` clause for `teams` construct and `num_threads` clause for `parallel` construct can be used to specify number of teams and threads
  - Performance is most likely not portable and compiler might just ignore these

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams num_teams(32)
#pragma omp parallel num_threads(128)
{
  // This block is executed in device
}
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams num_teams(32)
!$omp parallel num_threads(128)
  ! This block is executed in device
!$omp end parallel
!$omp end teams
!$omp end target
```
:::
::::::


# Composite directives

- In many cases composite directives are convenient
  - Enables also parallelizing a single loop over both teams and threads

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams distribute parallel for
for (int i = 0; i < N; i++) {
  p[i] = v1[i] * v2[i]
}
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams distribute parallel do
do i = 1, N
  p(i) = v1(i) * v2(i)
end do
!$omp end teams distribute parallel do
!$omp end target
```
:::
::::::


# Collapsing loops

- The `collapse(n)` clause can be used to combine *n* nested loops to a single large loop

::::::{.columns}
:::{.column}
```c++
#pragma omp target
#pragma omp teams distribute parallel for \
            collapse(2)
for (int i = 0; i < N; i++)
  for (int j = 0; j < M; j++) {
    ...
  }
```
:::
:::{.column}
```fortranfree
!$omp target
!$omp teams distribute parallel do &
!$omp     collapse(2)
do i = 1, N
  do j = 1, M
    ...
  end do
end do
!$omp teams distribute parallel do
!$omp end target
```
:::
::::::

# Loop construct

- Tells the compiler/runtime only that the loop iterations are independent and can be executed in parallel
  - Leaves more freedom to the implementation to do the work division
- Compiler support **might be buggy**, check that you get correct results and expected performance!

<div class=column>
```c++
#pragma omp target
#pragma omp teams loop
for (int i = 0; i < N; i++) {
  p[i] = v1[i] * v2[i]
}
```
</div>

<div class=column>
```fortranfree
!$omp target
!$omp teams loop
do i = 1, N
  p(i) = v1(i) * v2(i)
end do
!$omp end teams loop
!$omp end target
```
</div>


# OpenMP constructs for controlling data movement {.section}

# OpenMP data environment

- GPU device can have a separate memory from the host CPU
- By default, all data is on host, and it needs to be mapped to the device to be accessible inside the offloaded target region
- Variables are implicitly mapped to a target region unless explicitly defined
- Default implicit behavior:
  - Scalars: copied to the device as a part of the kernel launch (to GPU registers; no copy back)
  - Arrays: compiler tries to copy to the device and back (not possible for pointers as their size is not known!)

# Example: Implicit mapping

::::::{.columns}
:::{.column}
```c++
int n=1000;
double a=3.14;
double x[n], y[n];
// some code to initialise x and y

#pragma omp target
#pragma omp teams distribute parallel for
for (int i=0; i < n; i++) {
    y[i] += a * x[i];
}
```
:::
:::{.column}
```fortranfree
integer :: n
real(8) :: a
real(8), dimension(n) :: x, y
! some code to initialise x and y

!$omp target
!$omp teams distribute parallel do
do i = 1, n
  y(i) = a * x(i) + y(i)
end do
!$omp end teams distribute parallel do
!$omp end target
```
:::
::::::

- The scalar `a` is copied to the device as a part of the kernel launch
- The arrays `x` and `y` are copied to the device when the `target` region is entered (if the compiler knows their size) and back when it is exited

# Explicit mapping

**`target map(type: var1, var2, ...)`**

- Define data mapping for the associated target region
- The mapping `type` is one of:
  - `to`: copy data to device upon entry to the region
  - `from`: copy data from device upon exit from the region
  - `tofrom`: copy data to device upon entry and back on exit
  - `alloc`: allocate on the device (no copy, uninitialised)
- Mapping arrays:
  - C: `data[first index : size]`
  - Fortran: `data(first index : inclusive last index)`

# Example: Explicit mapping

::::::{.columns}
:::{.column}
```c++
int n=1000;
double a=3.14;
double x[n], y[n];
// some code to initialise x and y

#pragma omp target map(to: x[0:n]) \
                   map(tofrom: y[0:n])
#pragma omp teams distribute parallel for
for (int i=0; i < n; i++) {
    y[i] += a * x[i];
}
```
:::
:::{.column}
```fortranfree
integer :: n
real(8) :: a
real(8), dimension(n) :: x, y
! some code to initialise x and y

!$omp target map(to: x(1:n)) &
!$omp        map(tofrom: y(1:n))
!$omp teams distribute parallel do
do i = 1, n
  y(i) = a * x(i) + y(i)
end do
!$omp end teams distribute parallel do
!$omp end target
```
:::
::::::

- The scalar `a` is copied to the device as a part of the kernel launch
- The arrays `x` and `y` are copied to the device and only `y` is copied back


# Controlling data movement

- When dealing with a GPU device with separate memory, **optimizing data movement** is often **essential** to achieve good performance
- More elaborate data movement constructs discussed in the next lecture


# OpenMP runtime library routines {.section}

# OpenMP runtime library routines

- OpenMP provides runtime library routines in header / module:
  - C/C++: `#include <omp.h>`
  - Fortran: `use omp_lib`
- Some useful routines:
  - `omp_get_team_num()` returns the team id of the calling thread
  - `omp_get_num_teams()` returns the total number of teams
  - `omp_get_thread_num()` returns the thread id (in the team) of the calling thread
  - `omp_get_num_threads()` returns the total number of threads in the team
  - `omp_get_wtime()` returns the elapsed wall clock time in seconds


# OpenMP conditional compilation

- The `_OPENMP` macro is defined when OpenMP is enabled
- Use the macro to compile different code with and without OpenMP:

```cpp
#ifdef _OPENMP
  // This code is compiled when OpenMP is enabled (e.g. OpenMP library calls)
#else
  // This code is compiled when OpenMP is not enabled
#endif
```

# OpenMP environment variables {.section}

# OpenMP environment variables

- OpenMP standard defines a set of environment variables
- The environment variables are set before the program execution and they are read during the program start-up
  - Changing the variables during the execution has no effect


# Some useful environment variables

| Variable                  | Action                                              |
| ------------------------- | --------------------------------------------------- |
| `OMP_NUM_TEAMS`           | Set the maximum number of teams to use              |
| `OMP_TEAMS_THREAD_LIMIT`  | Set the maximum number of threads to in a teams     |
| `OMP_NUM_THREADS`         | Set the number of threads to use                    |
| `OMP_DISPLAY_ENV`         | Print the OpenMP environment info to stderr         |


# Summary {.section}

# Summary

- OpenMP enables directive-based programming of GPUs and other accelerators with C/C++ and Fortran
- Host--device model
  - Host offloads computations to the device
  - Host controls copying memory to/from the device
- Key constructs: `target teams distribute parallel for/do map(...)`
- Execution can be controlled with environment variables
