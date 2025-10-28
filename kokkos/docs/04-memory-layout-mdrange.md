---
title: Performance portable memory access patterns 
event: Portable GPU Programming 2025
date: October 2025
lang:     en
---

# Learning objectives

- Understand performance aspects of memory access patterns
- How View is layed out in memory
- How to mirror and copy Views
- How to iterate over tightly nested loops with MDRangePolicy

# Performance aspects of memory access

- Optimal memory access pattern is different in CPUs and in GPUs 
- CPU benefits from cached access
    - if thread t’s current access is at position i,
thread t’s next access should be at position i+1
- GPU benefits from coalesced access
    - if thread t’s current access is at position i,
thread t+1’s current access should be at position i+1
- Non-optimal access pattern may have large performance impact (more than 10 x)

# Performance aspects of memory access

- In order to achive good performance on different architectures, Kokkos maps iteration indices to
  threads depending on the execution space
```
parallel_for(N, KOKKOS_LAMBDA (const int idx) {...})
```
- On CPU execution spaces indices are given to threads in contiguous chunks:
    - Thread 0 gets idx=0, 1, 2, 3, ..., 
- On GPU execution spaces indices are given to threads strided:
    - idx=0 goes to thread 0, idx to thread 1, ...

# Memory layout of multidimensional View

- Multidimensional data can be mapped into the linear physical memory in different ways
- "left" layout (aka column major as in Fortran): consecutive entries in the same column of a 2-D array are contiguous in memory
- "right" layout (aka row major as in C/C++): consecutive entries in the same row of a 2-D array are contiguous in memory
- "strided" layout: each dimension has a stride

# How to choose optimum memory layout?

- Example: matrix vector product $y(j) = \sum_i A(j, i) * x(i)$
```c++
Kokkos::View<double**> A("A", n, m);
Kokkos::View<double*> y("y", n), x("x), m);

parallel_for("Ax", n, KOKKOS_LAMBDA (const int j) {
  y(j) = 0;
  for (int i=0, i < m; i++) {
    y(j) += A(j, i) * x(i);
  });
```
- On CPU right layout for A would be optimal
- On GPU left layout for A would be optimal

# Choosing layout of View

- Kokkos selects a View’s default layout for optimal parallel access based on its execution space
    - CUDA/HIP uses `LayoutLeft` to ensure *coalesced access*
    - OpenMP uses `LayoutRight` so that single thread accesses contiguous entries
    - Views on HostSpace are `LayoutRight`
- User may also specify layout when constructing a View:

```
Kokkos::View<double**, Kokkos::LayoutLeft> a ("a", n, m);
```

- View's layout can be queried from its `array_layout` typedef
- Defaults work when one is parallelizing over leftmost index

# Copying between different layouts

- Kokkos does not allow deep copies of Views with different layouts between different execution spaces
```
Kokkos::View<double**> a("a", n, m)
Kokkos::View<double**, Kokkos::HostSpace> b("a", n, m)

Kokkos::deep_copy(a, b) // Error if a is in CUDA/HIP memory space
```
- How to deal with the common need to copy data between host and device?

# Mirror Views

- Kokkos allows one to create "mirror" Views, *i.e.* Views with the same dimensions and **layout**
  as given View
    - By default the mirror view is in Host memory space

```
Kokkos::View<double**> d_a ("device a", n, m);
auto h_a = Kokkos::create_mirror_view(d_a) // returns d_a if d_a is already accessible
auto h_a2 = Kokkos::create_mirror(d_a) // Always allocate h_a2
Kokkos::deep_copy(h_a, d_a)  // No op if d_a was already accessible
```

# Subview

- Kokkos allows one to work with slices (similar to Python and Fortran) via *subviews*.
- Subview is always reference, *i.e.* modifying data via subview modifies also the orginal array
- Slices are defined with `std::make_pair`
- A special `Kokkos::ALL()`
```
Kokkos::View<double**> a ("a", 10, 10);
// a(2:4, 3:7) slice
auto a_slice = Kokkos::subview(a, std::make_pair(2, 4), std::make_pair(3, 7));
// a(:, 5) slice
auto a_slice2 = Kokkos::subview(a, Kokkos::ALL(), 5);
```

# Parallelizing over tighly nested loops

- Consider tightly nested loops such as
```
 for (int i = 1; i < nx - 1; i++) {
    for (int j = 1; j < ny - 1; j++) {
      for (int k = 1; k < nz - 1; k++) {
            curr(i, j, k)= ...
```
- With Kokkos construcst so far we could use `RangePolicy`

# Parallelizing over tighly nested loops

- Using RangePolicy:
```
parallel_for(Kokkos::RangePolicy(1, nx - 1), KOKKOS_LAMBDA (const int i) {
    for (int j = 1; j < ny - 1; j++) {
      for (int k = 1; k < nz - 1; k++) {
            curr(i, j, k)= ...
```
- Might be ok if `nx` is large enough, however, leaves out parallelization possibilities

# Parallelizing over tighly nested loops

- Kokkos provides also MDRangePolicy, which provides iteration range over up to 6 dimensions

```
parallel_for(Kokkos::MDRangePolicy<Kokkos::Rank<3>>({1, 1, 1,}, {nx -1, ny -1 , nz - 1}),
             KOKKOS_LAMBDA (const int i, const int j, const int k) {
                curr(i, j, k)= ...
```

- Lambda needs to be provided with the same number of iterations indeces than the rank of MDRange

# Summary

- Memory access pattern may have large impack on performance
    - *cached* for CPUs
    - *coalesced* for GPUs
- Kokkos provides performance portable memory access by having different execution policy 
  and default memory layout for CPUs and GPUs
    - LayoutRight for CPUs
    - LayoutLeft for GPUs
- Mirror views provide portable approach for deep copies between memory spaces that
  may have different default layouts
- Nested loops can be iterated with MdRangePolicy
