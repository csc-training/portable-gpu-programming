---
title:  Controlling data movement
event:  Portable GPU Programming 2025
lang:   en
---

# Outline

- OpenMP data environment
- Structured data region
- Unstructured data region


# OpenMP data environment {.section}

# OpenMP data environment

- GPU device can have a separate memory from the host CPU
  - Unified memory is accessible from both sides
  - OpenMP supports both unified and separate memory
- OpenMP includes constructs and clauses to
  ***allocate variables on the device*** and to define
  ***data transfers to/from the device***
- Data needs to be mapped to the device to be accessible inside the offloaded target region
  - Host is not allowed to touch the mapped data during the target region
  - Variables are implicitly mapped to a target region unless explicitly defined in a data clause
    - Scalars as *firstprivate*, static arrays copied to/from device


# Motivation for optimizing data movement

- When dealing with an accelerator GPU device attached to a PCIe bus, **optimizing data movement** is often **essential** to achieve good performance
- The four key steps in porting to high-performance accelerated code
  1. Identify parallelism
  2. Express parallelism
  3. Express data movement
  4. Optimise loop performance
  5. Go to 1!


# Example: implicit mapping

<div class=column>
```c++
int N=1000;
double a=3.14;
double x[N], y[N];
// some code to initialise x and y

#pragma omp target
#pragma omp teams distribute parallel for
for (int i=0; i < N; i++) {
    y[i] += a * x[i];
}
```
</div>

<div class=column>
```fortranfree
integer :: N
real(8) :: a
real(8), dimension(N) :: x, y
! some code to initialise x and y

!$omp target
!$omp teams distribute parallel do
do i = 1, N
  y(i) = a * x(i) + y(i)
end do
!$omp end teams distribute parallel do
!$omp end target
```
</div>


- Implicit copy of `a`, `x`, and `y` to the device when the `target` region is opened and back when it is closed


# Explicit mapping

`#pragma omp target map(type:list)`
  : `-`{.ghost}

- Explicit mapping can be defined with the `map` clause of the `target`
  construct
    - *list* is a comma-separated list of variables
    - *type* is one of:

<small>
<div class=column style="width:45%; margin-left:8%">
`to`
  : copy data to device on entry

`from`
  : copy data from device on exit
</div>
<div class=column style="width:45%">
`tofrom`
  : copy data to device on entry and back on exit

`alloc`
  : allocate on the device (uninitialised)
</div>
</small>


# Example: explicit mapping

<div class=column>
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
</div>

<div class=column>
```fortranfree
integer :: n
real(8) :: a
real(8), dimension(n) :: x, y
! some code to initialise x and y

!$omp target map(to: x(1:n)) map(tofrom: y(1:n))
!$omp teams distribute parallel do
do i = 1, n
  y(i) = a * x(i) + y(i)
end do
!$omp end teams distribute parallel do
!$omp end target
```
</div>

- Both `x` and `y` are copied the device, but only `y` is copied back to the host
- **Note!** Syntax in C: `data[first index : size]`<br>and in Fortran: `data(first index : inclusive last index)`

# Data regions {.section}

# Structured data region

- Define data mapping for a structured block that may contain multiple target regions
  - C/C++: `#pragma omp target data map(type:list)`
  - Fortran: `!omp target data map(type:list)`
  - Only maps data, one still needs to define a target region to execute code on the device
- Data transfers take place
  - From **the host** to **the device** upon entry to the region
  - From **the device** to **the host** upon exit from the region

# Example: data mapping over multiple target regions

<div class=column>
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
</div>
<div class=column>
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
</div>



# Update

- Update a variable within a data region with the `update` directive
  - C/C++: `#pragma omp target update type(list)`
  - Fortran: `!omp target update type(list)`
  - A single line executable directive
- Direction of data transfer is determined by the *type*, which can be either `to` (= copy to device) or `from` (= copy from device)

# Why update?

- Useful for producing snapshots of the device variables on the host or for updating variables on the device
  - Pass variables to host for visualization
  - Communication with other devices on other computing nodes
- Often used in conjunction with
  - Asynchronous execution of OpenMP constructs
  - Unstructured data regions


# Example: update within a data region

<div class=column>
```c++
#pragma omp target data map(to: x[0:n])
{
    #pragma omp target map(tofrom: y[0:n])
    #pragma omp parallel for
    for (int i=0; i < n; i++)
        y[i] += a * x[i];

    // ... some host code that modifies x ...
    #pragma omp target update to(x[0:n])

    #pragma omp target map(tofrom: z[0:n])
    #pragma omp parallel for
    for (int i=0; i < n; i++)
        z[i] += b * x[i];
}
```
</div>
<div class=column>
```fortranfree
!$omp target data map(to: x(1:n))
  !$omp target map(tofrom: y(1:n))
  !$omp teams distribute parallel do
  do i = 1, n
    y(i) = a * x(i) + y(i)
  end do
  !$omp end teams distribute parallel do
  !$omp end target

  ! ... some host code that modifies x ...
  !$omp target update to(x(1:n))

  !$omp target map(tofrom: z(1:n))
  !$omp teams distribute parallel do
  do i = 1, n
    z(i) = b * x(i) + z(i)
  end do
...
```
</div>



# Unstructured data regions

- Unstructured data regions enable one to handle cases where memory allocation and deallocation is done in a different scope
  - Useful for e.g. C++ classes, Fortran modules
- `enter data` defines the start of an unstructured data region
  - C/C++: `#pragma omp enter data [clauses]`
  - Fortran: `!$omp enter data [clauses]`
- `exit data` defines the end of an unstructured data region
  - C/C++: `#pragma omp exit data [clauses]`
  - Fortran: `!$omp exit data [clauses]`


# Unstructured data

```c++
class Vector {
    Vector(int n) : len(n) {
        v = new double[len];
        #pragma omp enter data alloc(v[0:len])
    }
    ~Vector() {
        #pragma omp exit data delete(v[0:len])
        delete[] v;
    }
    double v;
    int len;
};
```

# Enter data clauses

`map(alloc:var-list)`
  : Allocate memory on the device

`map(to:var-list)`
  : Allocate memory on the device and copy data from the host to the device


# Exit data clauses

`map(delete:var-list)`
  : Deallocate memory on the device

`map(from:var-list)`
  : Deallocate memory on the device and copy data from the device to the host


# Summary

- GPU device can have a separate memory space from the host CPU
- Implicit transfer of data happens to/from device if not explictly defined
- Explicit data mapping gives more control and enables better performance
- Data regions enable controlling data movement over multiple kernel executions
  - Updating to/from device is possible within data region
