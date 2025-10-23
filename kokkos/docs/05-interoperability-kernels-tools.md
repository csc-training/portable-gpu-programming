---
title:  Kokkos C++ Performance Portability Ecosystem
event: Portable GPU Programming 2025
date: October 2025
lang:     en
---

# Interoperability with libraries and other frameworks {.section}

# Learning objectives

- How to pass data from Kokkos Views
- How to use Kokkos Views with externally managed data
- How to use Kokkos together with MPI 

# Kokkos Interoperability

- Generally, Kokkos provides good support for interoperability with various "external stuff" 
- Fortran and Python (not discussed here)
- External libraries
- Underlying (and even other) backend models
    - OpenMP, CUDA, HIP, ...
- MPI

# Passing data from Kokkos

- In many cases, only thing needed is to obtain the raw pointer to the data in the Kokkos View.
- Can be achieved with the `data()` method of View
```
Kokkos::View<float**> a("a", n, m);
auto a_ptr = a.data();

some_external_function(a_ptr, n, m, ...)
```
- Memory layout may need special considerations

# Combining Kokkos with low level code

- Kokkos allow to include e.g. CUDA/HIP code even in the same source with the Kokkos code
- Might be used for special low level optimization
```c++
Kokkos::View<float**> a("a", n, m);
...
#ifdef KOKKOS_ENABLE_CUDA

__global__ void my_cuda_kernel(float *a, int n, int m) {};

// CUDA grid and block need to be defined
my_cuda_kernel<<<grid, block>>>(a.data(), n, m);
#else
parallel_for(...);
#endif
```

# Using Kokkos Views with externally managed data
 
- Sometimes Kokkos code might be provided data that has been allocated outside Kokkos
- Kokkos allows to wrap this kind of data inside an Unmanaged View
    - No reference counting (and automatic deallocation)
    - No check for correct memory space
- Unmanaged View can be created by passing a raw pointer as first argument to View
    - Memory space, dimensions and layout need to be also specified
    - No label

```c++
View<double**, Kokkos::LayoutRight, Kokkos::HostSpace> a(a_ptr, n, m);
```

# Using MPI together with Kokkos

- In large extent using MPI and Kokkos together is as simple as passing the raw pointer of Kokkos::View
  to MPI routines
    - Note that the data needs to be contiguous in memory
- MPI should be initialized before Kokkos
- Kokkos maps always node local MPI ranks to GPUs in round-robin
- For building MPI applications it is usually enough to add to `CMakeLists.txt`

```
find_package(MPI REQUIRED)
target_link_libraries(myapp PRIVATE MPI::MPI_CXX)
```

# Summary

- Raw pointer of the data in View can be obtained with the `data()` method
    - Pointer can be passed as such to non-Kokkos code
- Pointers allocated outside Kokkos can be wrapped into an Unmanaged View
    - No automatic deallocation
- MPI can be used Kokkos code just by passing the raw pointer of View to MPI routines
    - Kokkos takes care of mapping MPI tasks to GPUs

# Kokkos Kernels and Tools {.section}

# Kokkos Kernels

- Mathematical kernels for common dense and sparse linear algebra, and graph problems
- Portable interface to vendor libraries operating on Kokkos views
    - MKL, cuBLAS, rocBLAS, ...  
- Custom implementation for special cases e.g. skinny matrices 
- Batched operations for small problem sizes
- Needs to be installed separately from Kokkos Core
- See <https://kokkos.org/kokkos-kernels/docs> for available kernels

# Dot product with Kokkos Kernels
 
```
#include <KokkosBlas1_dot.hpp>

Kokkos::View<double*> x("x", n), y("y", n);

auto result = KokkosBlas::dot(x, y) ;
```

In `CMakeLists.txt`
```
find_package(KokkosKernels REQUIRED)

target_link_libraries(gemm Kokkos::kokkoskernels)
```
- No separate CMake setting is needed for Kokkos core (i.e. `find_package(Kokkos)` 
 
# Kokkos Tools {.section}

# Kokkos Tools

- Tools is a separate component in the Kokkos ecosystem
- Provides utilities to help in debugging
- Simple profiling and memory usage monitoring
- Hooks for external profiling tools
    - Intel VTune, NVIDIA tools, AMD tools, TAU, ...
- Implemented as set of shared libraries, used by setting `KOKKOS_TOOLS_LIBS` environment variable
- Provides also explicit instrumentation API to be used in application
- Documentation at <https://github.com/kokkos/kokkos-tools/wiki> 

# Debugging

- Due to asynchronous nature of parallel dispatch it is not always clear where application crashes
- Kernel Logger prints start and ends of kernels
    - Also synchronizes at the end of the kernels 
- Usage:

```
export KOKKOS_TOOLS_LIBS={PATH_TO_TOOL_DIRECTORY}/libkp_kernel_logger.so
srun myapp
```

# Profiling

- A simple flat profile of Kokkos application can be obtained with the Simple Kernel Timer tool
- Usage:

```
export KOKKOS_TOOLS_LIBS={PATH_TO_TOOL_DIRECTORY}/libkp_kernel_timer.so
srun myapp
```

- Result is a binary `nodename-processid.dat` file that can be investigated with the included `kp_reader`
program

```
kp_reader nid002275-210818.dat
```

# Profiling

- Kokkos kernels show up in external profiling tools as long hard-to-interpret Kokkos function calls
- Kokkos tools provides "hooks" which insert e.g. roctx and nvtx markers show that Kokkos kernels show up 
  with the "name"s they were given
- AMD GPUs (Roctx): `libkp_roctx_connector.so`
    - Need to include roctx trace, e.g. by using `--roctx-trace` option for `rocprof`
- NVIDIA GPUs (Nvtx): `libkp_nvtx_focused_connector.so`
- Note: the library names ("\*so")in documentation seem to miss the "lib"-prefix,
  check PATH_TO_TOOL_DIRECTORY for available tools 

# Summary

- Kokkos Kernels provides common linear algebra and graph operations
- Portable interfaces to vendor libraries
- Custom implementations for special cases
- Kokkos tools provides utilities for debugging and profiling
    - Used via `KOKKOS_TOOLS_LIBS` environment variable

# Further topics {.section}

# Further topics

- Kokkos has a lot of features not discussed during this course
- Asynchronous execution
- Atomic operations
- Hierarchical parallelism
    - Create thread teams and parallelize also within the teams
    - "scratch memory" within a team

# Further topics

- Algorithms (random numbers, sorting, std algorithms, ...) 
- Task graphs
- ...

- See <https://kokkos.org/kokkos-core-wiki> for more details

# Summary

- Kokkos is a performance portable C++ programming ecosystem supporting various hardware architectures
    - OpenMP, CUDA, HIP, ... backends
- Kokkos abstracts the hardware into execution and memory spaces
- Kokkos provides a multidimensional array data structure View that can reside on different memory spaces
- Kokkos provides parallel dispatch operations that execute computational kernels in execution spaces
