<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Asynchronous kernel execution
event: Portable GPU Programming 2026
lang:  en
---

# Outline

- Asynchronous kernel execution
- Overlapping CPU and GPU execution


# Asynchronous kernel execution {.section}

# Synchronous vs asynchronous execution

- By default in OpenMP offload, execution in the host continues only after the target region has finished
  - CPU is idling until the GPU has finished
- With the `nowait` clause, the host thread continues immediately after the work has been submitted to the device
  - Explicit synchronization needed on host with `taskwait`

# Benefits of asynchronous execution

- Frees up the host CPU to perform other tasks while the device executes the offloaded region
- Reduces the latency of kernel launches, which is especially useful for short kernels or pipelines or iterative workloads


# Example

<div class=column>
```c++
// Launch kernels asynchronously
#pragma omp target nowait
{  ...
   x[i] = a * u[i];
}

#pragma omp target nowait
{  ...
   y[i] = b * v[i];
}

// Wait all kernels to finish
#pragma omp taskwait
```
</div>

<div class=column>
```fortranfree
! Launch kernels asynchronously
!$omp target nowait
   ...
   x(i) = a * u(i)
!$omp end target

!$omp target nowait
   ...
   y(i) = b * v(i)
!$omp end target

! Wait all kernels to finish
!$omp taskwait
```
</div>


# Controlling execution order

- If the kernels are sufficiently small, the device might execute multiple of them simultaneously
  - Similarly to using multiple streams in CUDA/HIP
- The `depend` clause can be used to specify constraints on the execution order
  - Same way as with OpenMP tasks

# Controlling dependencies

- If the kernels are sufficiently small, the device might execute multiple of them simultaneously
- This is not desired for all workloads, so one needs to order them by using `depend` clause
  - Allows fine-grained scheduling of kernels that share data. No need for `taskwait` too often!

# Dependency variables

- Dependency (`in`, `out`, and `inout`) is associated with the memory address of a variable
  - Rule: `in`/`inout` tasks must execute after any **previously created** `out` / `inout` tasks
  - Meaning of names:
    - `depend(in: x[0:n])`: the target region will read from `x` (i.e., `x` is input)
    - `depend(out: x[0:n])`: the target region will write to `x` (i.e., `x` is output)
    - `depend(inout: x[0:n])`: the target region will read from and write to `x` (i.e., `x` is input and output)
  - Note! The dependency variables can also be dummy variables used solely for ordering the tasks (no real read/write is necessary)

# Example

:::{.column}
```c++
// Launch kernels asynchronously
#pragma omp target nowait depend(out: x[0:n])
{  ...
   x[i] = a * u[i];
}

#pragma omp target nowait
{  ...
   y[i] = b * v[i];
}

#pragma omp target nowait depend(in: x[0:n])
{  ...
   z[i] = c * x[i];
}

// Wait all kernels to finish
#pragma omp taskwait
```
:::
:::{.column}
```fortranfree
! Launch kernels asynchronously
!$omp target nowait depend(out: x(1:n))
   ...
   x(i) = a * u(i)
!$omp end target

!$omp target nowait
   ...
   y(i) = b * v(i)
!$omp end target

!$omp target nowait depend(in: x(1:n))
   ...
   z(i) = c * x(i)
!$omp end target

! Wait all kernels to finish
!$omp taskwait
```
:::


# Overlapping CPU and GPU execution {.section}

# Motivation

- Compute nodes have often multiple CPU cores available for each GPU
  - Roihu GH200 superchip has 72 CPU cores for the GPU
- Asynchronous GPU execution enables utilizing CPU resources simultanously
  - Example here: using CPU threads via OpenMP tasks together with GPU

# CPU multithreading with OpenMP

- The traditional way of using OpenMP
- No enclosing `target` construct &rarr; `parallel` creates CPU threads

<div class=column>
```c++
#pragma omp parallel [clauses]
{
  // This block is executed with
  // multiple CPU threads in parallel

}
```
</div>

<div class=column>
```fortranfree
!$omp parallel [clauses]

  ! This block is executed with
  ! multiple CPU threads in parallel

!$omp end parallel
```
</div>

# Task in OpenMP

- A similar packaged computation work as GPU kernel, but executed by a CPU thread
- A task has
  - Code to execute
  - Data environment
  - Internal control variables
- Tasks are added to a task queue, and executed then by any single CPU thread
  - OpenMP runtime takes care of distributing tasks to threads
  - Execution may be deferred or started immediately after tasks is created


# OpenMP task construct

- Create a new task and add it to task queue
  - Store data and code to be executed
  - Task constructs can be arbitrarily nested

<div class=column>
```cpp
#pragma omp task [clause[[,] clause], ...]
{
  ...
}
```
</div>
<div class=column>
```fortranfree
!$omp task [clause[[,] clause], ...]

...
!$omp end task
```
</div>


# OpenMP task construct

- All threads that encounter the construct create a task
- Typical usage pattern is thus that a single thread creates the tasks

<div class=column>
```cpp
#pragma omp parallel  // Create host threads
#pragma omp single  // Single thread to orchestrate work
{
    #pragma omp task depend(out: a)
    {
        a = -1;
    }

    #pragma omp task depend(in: a)
    {
        b = 2 * a;
    }
}
```
</div>
<div class=column>
```fortranfree
!$omp parallel  ! Create host threads
!$omp single  ! Single thread to orchestrate work

  !$omp task depend(out: a)
    a = -1;
  !$omp end task

  !$omp task depend(in: a)
    b = 2 * a;
  !$omp end task

!$omp end single
!$omp end parallel
```
</div>


# Task execution model

- Tasks are executed by an arbitrary thread
    - Can be same or different thread that created the task
    - By default, tasks are executed in an arbitrary order
    - Each task is executed only once
- Synchronisation points
    - Implicit or explicit barriers
    - `#pragma omp taskwait / !$omp taskwait`
        - Encountering task suspends until child tasks complete


# Data environment of a task

- Variables that are `private` in the enclosing construct are made
  `firstprivate` and contain the data at the time of creation
- Variables that are `shared` in the enclosing construct contain the data at
  the time of execution
- Data scoping clauses (`shared`, `private`, `firstprivate`, `default`) can
  change the default behaviour

# Data-sharing clauses

- **`private(var1, var2, ...)`**
  - Private variables are stored in the private stack of each thread
  - Undefined initial value
  - Undefined value after parallel region
- **`firstprivate(var1, var2, ...)`**
  - Same as `private`, but with an initial value that is the same as
    the original value defined outside the parallel region


# Data-sharing clauses

- **`shared(var1, var2, ...)`**
  - All threads can write to and read from a shared variable
- **`default(private/shared/none)`**
  - Sets default for variables to be shared, private or not defined
  - In C/C++, `default(private)` is not allowed
  - Setting `default(none)` can be useful for debugging as each variable has to be
    defined manually

# Combining GPU execution with OpenMP tasks

:::{.column}
```c++
#pragma omp parallel  // Create host threads
#pragma omp single  // Single thread to orchestrate work
{
  #pragma omp target nowait depend(...)
  { ... }

  #pragma omp target nowait depend(...)
  { ... }

  #pragma omp task depend(...)
  { ... }

  // Wait all kernels and host tasks to finish
  #pragma omp taskwait

} // end of host threads
```
:::
:::{.column}
```fortranfree
!$omp parallel  ! Create host threads
!$omp single  ! Single thread to orchestrate work
  !$omp target nowait depend(...)
    ...
  !$omp end target

  !$omp target nowait depend(...)
    ...
  !$omp end target

  !$omp task depend(...)
    ...
  !$omp end task

  ! Wait all kernels and host tasks to finish
  !$omp taskwait
!$omp end single
!$omp end parallel
```
:::


# Summary

- In OpenMP offload, device execution is synchronous with host by default
- Asynchronous execution is possible, but requires explicit synchronization
  and more care with the correct order of execution
- Asynchronous execution enables utilizing both CPU and GPU resources simultaneously
