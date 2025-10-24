---
title:  Asynchronous kernel execution
event:  Portable GPU Programming 2025
lang:   en
---

# Outline

- Asynchronous kernel execution


# Asynchronous kernel execution {.section}

# Synchronous vs asynchronous execution

- By default, execution in the host continues only after the target region has finished
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



# Controlling dependencies

- If the kernels are sufficiently small, the device might execute multiple of them simultaneously
- This is not desired for all workloads, so one needs to order them by using `depend` clause
  - `depend(in: x[0:n])`: the target region will read from `x`
  - `depend(out: x[0:n])`: the target region will write to `x`
  - `depend(inout: x[0:n])`: the target region will read from and write to `x`
- OpenMP runtime will execute the kernels in such an order that `x` is not read from before the kernel writing to it has finished

# Example

<div class=column>
```c++
// Launch kernels asynchronously
#pragma omp target nowait depend(out: x[1:n])
{  ...
   x[i] = a * u[i];
}

#pragma omp target nowait
{  ...
   y[i] = b * v[i];
}

#pragma omp target nowait depend(in: x[1:n])
{  ...
   z[i] = c * x[i];
}

// Wait all kernels to finish
#pragma omp taskwait
```
</div>

<div class=column>
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
</div>

# Combining with OpenMP tasks

- The target regions and their dependencies can be combined with OpenMP tasks
  - Not covered in detail here, but see an example code in exercises

# Summary

- In OpenMP offload, device execution is synchronous with host by default
- Asynchronous execution is possible, but requires explicit synchronization
  and more care with the correct order of execution
