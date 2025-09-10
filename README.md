# Portable GPU Programming

Course material for the CSC course "Portable GPU Programming". The course is
part of the EuroCC training activities at CSC.

## Presentation slides

The slides are available [here](https://csc-training.github.io/high-level-gpu-programming/).

## SYCL Book

[Data Parallel C++ Programming Accelerated Systems Using C++ and SYCL](https://link.springer.com/book/10.1007/978-1-4842-9691-2)

[Book Examples](https://github.com/Apress/data-parallel-CPP.git)

## Agenda

### Day 1, Tuesday 21.10

| Time         | Topic |
| ----         | ----- |
| 09:15-09:30  | Welcome
| 09:30-10:00  | Introduction to Parallel Computing
| 10:00-11:00  | Mahti and LUMI Computing Platforms
| 11:00-12:00  | C++ Refresher
| 12:00-13:00  | Lunch break
| 13:00-13:45  | Introduction to GPUs
| 13:45-15:00  | GPU execution model
| 15:00-15:30  | Coffee break
| 15:30-16:30  | GPU memory hierarchy
| 16:30-16:45  | Day 1 wrap-up

### Day 2, Wednesday 22.10

| Time         | Topic |
| ----         | ----- |
| 09:15-12:00  | SYCL I (with exercises)
| 12:00-13:00  | Lunch break
| 13:00-15:00  | SYCL II (with exercises)
| 15:00-15:30  | Coffee break
| 15:30-16:30  | SYCL III (with exercises)
| 16:30-16:45  | Day 2 wrap-up

### Day 3, Thursday 23.10

| Time         | Topic |
| ----         | ----- |
| 09:15-12:00  | Kokkos I (with exercises)
| 12:00-13:00  | Lunch break
| 13:00-15:00  | Kokkos II (with exercises)
| 15:00-15:30  | Coffee break
| 15:30-16:30  | Kokkos III (with exercises)
| 16:30-16:45  | Day 3 wrap-up

### Day 4, Friday 24.10

| Time         | Topic |
| ----         | ----- |
| 09:15-12:00  | OpenMP offloading I (with exercises)
| 12:00-13:00  | Lunch break
| 13:00-15:00  | OpenMP offloading II (with exercises)
| 15:00-15:30  | Coffee break
| 15:30-16:30  | OpenMP offloading III (with exercises)
| 16:30-16:45  | Day 4 wrap-up & closing

The lectures in this repository are published under [CC-BY-SA license](https://creativecommons.org/licenses/by-nc/4.0/). Some of the lectures and exercises are based on Intel copyrighted work and they have their own license ([MIT](https://spdx.org/licenses/MIT.html)).

## Exercises

[General instructions](Exercises_Instructions.md)

### Kokkos
- [Kokkos](/exercises/kokkos)

### SYCL Essentials
- [Hello World](/exercises/sycl/00-hello/)
- [Getting Device Info](/exercises/sycl/01-info/)
- [Vector Addition](/exercises/sycl/02-vector_add)

### SYCL Advanced Features
- [Dependencies](exercises/sycl/03-axpy/)
- [Basic Profiling](exercises/sycl/12-basic-profiling)
- [Error Handling](exercises/sycl/13-error-handling/)

### SYCL Memory Optimizations
- [Jacobi Iterations](exercises/sycl/07-jacobi)
- [Dense matrix matrix multiplication](exercises/sycl/04-matrix-matrix-mul)
- [Reductions](exercises/sycl/06-reduction-direct) (Optional)

### Multi-GPU programming with MPI
- [MPI send and receive demo](exercises/sycl/08-mpi)
- [Parallel calculation of π with multiple GPU](exercises/sycl/11-pi/)

### SYCL interoperability
- [SYCL and 3rd party libraries](exercises/sycl/09-interoperability/)
