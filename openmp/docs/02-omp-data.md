<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Controlling data movement
event: Portable GPU Programming 2026
lang:  en
---

# Outline

- Recap of data mapping for a single target region: `target map`
- Structured data region: `target data map`
- Unstructured data region: `target enter data map`, `target exit data map`

# Data mapping {.section}

# Motivation

- When dealing with a GPU device with separate memory, **optimizing data movement** is often **essential** to achieve good performance
  - Matrix multiplication with 10000 x 10000 matrices in LUMI:<br>
    Host-to-device memory copies: 0.34 s; Computation: 0.05 s

<center>
**Peak theoretical bandwidth**

|      | CPU-GPU bus | GPU memory |
|------|------------:|--------------:|
| Mahti: A100  | 32+32 GB/s | 1555 GB/s|
| LUMI: MI250X | 36+36 GB/s | 1600 GB/s|
| Roihu: GH200 | 450+450 GB/s | 3928 GB/s |
</center>


# Recap: Explicit mapping

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

# Structured data region

**`target data map(type: var1, var2, ...)`**

- Define data mapping for a structured block that may contain multiple target regions
  - Only maps data, one still needs to define a separate target region to execute code on the device
- The mapping `type` is like in `target map`
- **Note!** Do not confuse `target data` with `target`!

# Example: Data mapping over multiple target regions

::::::{.columns}
:::{.column}
```c++
#pragma omp target data map(to: x[0:n])
{
    #pragma omp target map(tofrom: y[0:n])
    #pragma omp teams distribute parallel for
    for (int i=0; i < n; i++)
        y[i] += a * x[i];

    #pragma omp target map(tofrom: z[0:n])
    #pragma omp teams distribute parallel for
    for (int i=0; i < n; i++)
        z[i] += b * x[i];
}
```
:::
:::{.column}
```fortranfree
!$omp target data map(to: x(1:n))
  !$omp target map(tofrom: y(1:n))
  !$omp teams distribute parallel do
  do i = 1, n
    y(i) = a * x(i) + y(i)
  end do
  !$omp end teams distribute parallel do
  !$omp end target

  !$omp target map(tofrom: z(1:n))
  !$omp teams distribute parallel do
  do i = 1, n
    z(i) = b * x(i) + z(i)
  end do
  !$omp end teams distribute parallel do
  !$omp end target
!$omp end target data
```
:::
::::::


# Update

**`target update type(var1, var2, ...)`**

- Update a variable within a data region
  - *A single-line executable directive* that triggers data copy (not associated with any code)
- The `type` is one of:
  - `to`: copy data to device
  - `from`: copy data from device
- Useful for producing snapshots of the device variables on the host or for updating variables on the device
- **Note!** Do not confuse `target update` with `target`!

# Example: Update within a data region

::::::{.columns}
:::{.column}
```c++
#pragma omp target data map(to: x[0:n])
{
    // Some GPU code that modifies x
    // ...

    // Copy x from device to host
    #pragma omp target update from(x[0:n])

    // Write x to disk on host
    // ...
}
```
:::
:::{.column}
```fortranfree
!$omp target data map(to: x(1:n))
  ! Some GPU code that modifies x
  ! ...

  ! Copy x from device to host
  !$omp target update from(x(1:n))

  ! Write x to disk on host
  ! ...
!$omp end target data
```
:::
::::::



# Unstructured data regions

**`target enter data map(type: var1, var2, ...)`**

- The mapping `type` is one of:
  - `alloc`: allocate on the device
  - `to`: copy data to device

**`target exit data map(type: var1, var2, ...)`**

- The mapping `type` is one of:
  - `delete`: deallocate on the device
  - `from`: copy data from device

- Unstructured data regions enable handling cases where memory allocation and deallocation is done in a different scope


# Example: Unstructured data region

```c++
class Vector {
    Vector(int n) : len(n) {
        v = new double[len];
        #pragma omp target enter data map(alloc: v[0:len])
    }
    ~Vector() {
        #pragma omp target exit data map(delete: v[0:len])
        delete[] v;
    }
    double v;
    int len;
};
```

# Summary {.section}
# Summary

- GPU device can have a separate memory space from the host CPU
- Implicit transfer of data happens to/from device if not explictly defined
- Explicit data mapping gives more control and enables better performance
- Data regions enable controlling data movement over multiple kernel executions
  - Updating to/from device is possible within data region
