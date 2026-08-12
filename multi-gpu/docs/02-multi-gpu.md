<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Using multiple GPUs with MPI
event: Portable GPU Programming 2026
lang:  en
---

# Outline

- Different approaches for multi-GPU usage
- Using multiple GPUs with OpenMP offloading and MPI
- Using multiple GPUs with Kokkos offloading and MPI

# Using multiple GPUs

- Why to use multiple GPUs?
    - Application requires more memory than a single GPU has
    - Solve a problem faster than possible with a single GPU
- In order to use multiple GPUs one needs to:
    - Coordinate the work between GPUs
    - Move data between GPUs
- MPI and RCCL/NCCL can be used both for intra- and internode data movement

# Computing with multiple GPUs

- A problem is split into smaller subtasks
- Multiple subtasks are processed simultaneously using multiple GPUs
    - standard OpenMP / Kokkos programming for each GPU

![](img/compp.svg){.center width=40%}

# Multi-GPU Programming Models

<div class="column">
* One GPU per process
    * syncing is handled through message passing (e.g. MPI)
* Many GPUs per process
    * process manages all context switching and syncing explicitly
* One GPU per thread
    * syncing is handled through thread synchronization requirements
</div>

<div class="column">
![](img/single_proc_mpi_gpu2.png){width=50%}
![](img/single_proc_multi_gpu.png){width=50%}
![](img/single_proc_thread_gpu.png){width=50%}
</div>

# One GPU per process

- Most common approach for HPC applications
- Communication between GPUs with MPI or NCCL/RCCL
- Works with arbitrary number of GPUs
    - same programming approach for inter- and intranode data movement
- Each process interacts with only one GPU which makes the implementation
  easier and less invasive (if MPI is used anyway)
- Very similar MPI programming as with CPUs

# Using multiple GPUs with MPI

- By default, each process sees all the GPUs
- In order to use different GPUs for different processes one needs to
  make a process specific selection
- If MPI implementation supports GPU aware communication (nowadays most do), one can pass 
  device pointers directly to MPI routines
    - Otherwise, data has to be explicitly copied between host and device for communication

# Using OpenMP offloading with MPI

- OpenMP provides interfaces for device management

<small>

| Description | API Call |
|-|-|
| Query the number of devices within a node | `int omp_get_num_devices()` |
| Set `device` as the current device for the calling host thread | `void omp_set_default_device(device)` |
| Query the current device for the calling host thread| `int omp_get_default_device()`  |

</small>

- GPU aware communication can be done with the help of `use_device_ptr` / `use_device_addr` directives
    - Note that the data needs to be contiguous in memory

# Using OpenMP offloading with MPI

```cpp
int deviceCount, nodeRank;
MPI_Comm commNode;
MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &commNode);
MPI_Comm_rank(commNode, &nodeRank);

deviceCount = omp_get_num_devices();
omp_set_default_device(nodeRank % deviceCount);

#pragma omp target data use_device_ptr(x)
{
  MPI_Sendrecv(x, ...);
  ...
}
```

- Program needs to compiled with MPI wrappers (e.g. `nvc` -> `mpicc`)

# Using Kokkos together with MPI

- Kokkos does not provide specific interfaces for device management 
- Kokkos automatically selects device based on the environment
    - Node local processes are assigned to available GPUs in a round robin fashion
- MPI should be initialized before Kokkos
- For communication one can just pass the raw pointer of Kokkos::View
  to MPI routines
    - Note that the data needs to be contiguous in memory

# Using Kokkos together with MPI

```cpp
MPI_Init(&argc, &argv);
Kokkos::initialize();
{
   ...
  MPI_Sendrecv(x.data(), ...);
}
```

- For building MPI applications it is usually enough to add to `CMakeLists.txt`

```
find_package(MPI REQUIRED)
target_link_libraries(myapp PRIVATE MPI::MPI_CXX)
```

# Summary

- Using MPI together with one GPU per MPI processes is often the most straightworfward and scalable way for using multiple GPUs
- With OpenMP, one needs to manage devices explicitly, Kokkos does it automatically
- Both OpenMP and Kokkos allow passing device pointers directly to MPI calls
    - If MPI implementation is not GPU aware, data has to be copied between host and device

