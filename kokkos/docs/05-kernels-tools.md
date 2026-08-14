<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Kokkos Kernels and Tools
event: Portable GPU Programming 2026
lang:  en
---


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
