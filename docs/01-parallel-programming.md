---
title:    Parallel Programming & GPUs
subtitle: Portable GPU Programming
date:     October 2024
lang:     en
---

# Parallel computing concepts {.section}

# Computing in parallel

<div class=column>
- Parallel computing
  - A problem is split into smaller subtasks
  - Multiple subtasks are processed simultaneously using multiple computing units
- Parallel overheads
  - Additional operations which are not present in serial calculation
  - Synchronization, redundant computations, communications

</div>
<div class=column>
<!-- Copyright CSC -->
 ![](img/compp.svg){.center width=100%}
</div>

# Types of parallel problems

- Tightly coupled
    - Lots of interaction between subtasks
    - Example: Weather simulation
    - Low latency, high speed interconnect is essential
- Embarrassingly parallel
    - Very little (or no) interaction between subtasks
    - Example: Sequence alignment queries for multiple independent sequences in bioinformatics


# Exposing parallelism: Data parallelism

<div class=column>
- Data parallelism
  - Data is distributed across cores
  - Each core performs simultaneously (nearly) identical operations with different data
  - Cores may need to interact with each other, e.g. exchange information about data on domain boundaries
</div>
<div class=column>

<!-- Copyright CSC -->
 ![](img/eparallel.svg){.center width=80%}

</div>


# Exposing parallelism: Tasking

- Task farm (main / worker)

<!-- Copyright CSC -->
 ![](img/farm.svg){.center width=60%}

<br>

- Main worker sends tasks to workers and receives results
- There are normally more tasks than workers, and tasks are assigned dynamically


# Parallel algorithms {.section}

# What can be calculated in parallel?

There needs to be independent computations<br><br>

<div class=column>
Gauss-Seidel iteration:
```
while True:
  for i:
    u[i] = (u[i-1] + u[i+1]
            - h**2 * f[i]) / 2

until converged(u)
```

Loop cannot be parallelized over `i` due to data dependency

</div>
<div class=column>
Jacobi iteration:
```
while True:
  for i:
    u_new[i] = (u_old[i-1] + u_old[i+1]
                - h**2 * f[i]) / 2
  swap(u_new, u_old)
until converged(u)
```

Loop can be parallelized over `i`

</div>

# What happens when there is  competition over the same data?

- Race condition
    - Two (or more) processes access shared data concurrently
    - The final outcome depends on the sequence or timing of execution
    - Unpredictable and often leads to bugs
    - Example: Two threads incrementing the same counter simultaneously might overwrite each other’s result

- Deadlock
    - Two (or more) processes wait indefinitely for each other to release resources (or e.g. to send data)
    - System halts or stalls due to resource unavailability


# Data distribution: local vs. global dependencies

<div class=column>
Local dependencies

- Stencils: 
  ```
  v[i,j] = u[i-1, j] + u[i+1, j] + u[i, j-1] ...
  ```
- Finite element methods
- Particle based methods with short range interactions
- Number of communications per process remains constant
</div>
<div class=column>
Global dependencies

- Fourier transform 
  $X_k = \sum_{n=0}^{N-1} x_n \exp(-i \frac{2\pi}{N}k \cdot n)$
- Linear algebra: $C = A \times B$

  <!-- Copyright CSC -->
  ![](img/matmul.png){.center width=50%}

- Number of communication events per process increases with number of execution units

# Data distribution: load balance

<div class=column>
Simple domains with similar computational cost

<br>
<!-- Copyright CSC -->
![](img/eparallel.svg){.center width=50%}
</div>

<div class=column>
Simple domains with different computational cost

<br>
<!-- Copyright CSC -->
![](img/mandelbrot-domains.png){.center width=40%}
</div>


# Parallel programming models {.section}

# Parallel programming models

- Parallel execution is based on threads or processes (or both) which run at the same time on different CPU cores
- Processes
    - Interaction is based on exchanging messages between processes
    - MPI (Message passing interface)
- Threads
    - Interaction is based on shared memory, i.e. each thread can access directly other threads data
    - OpenMP, pthreads

# Parallel programming models

<!-- Copyright CSC -->
 ![](img/processes-threads.svg){.center width=80%}
<div class=column>
**MPI: Processes**

- Independent execution units
- MPI launches N processes at application startup
- Works over multiple nodes
</div>
<div class=column>

**OpenMP: Threads**

- Threads share memory space
- Threads are created and destroyed  (parallel regions)
- Limited to a single node

</div>

# GPU programming models

- GPUs are co-processors to the CPU
- CPU controls the work flow:
  - *offloads* computations to GPU by launching *kernels*
  - allocates and deallocates the memory on GPUs
  - handles the data transfers between CPU and GPUs
- GPU kernels run multiple threads
    - Typically much more threads than "GPU cores"
- When using multiple GPUs, CPU runs typically multiple processes (MPI) or multiple threads (OpenMP)

# GPU programming models

![](img/gpu-offload.svg){.center width=40%}
<br>

- CPU launches kernel on GPU
- Kernel execution is normally asynchronous
    - CPU remains active
- Multiple kernels may run concurrently on same GPU


# Supercomputers {.section}


# Anatomy of a supercomputer

<!-- Copyright CSC -->
![](img/cluster_diagram.svg){.center width=100%}


# Current supercomputers

<div class=column>
- Large computer clusters
- A lot of standard, high-end server hardware connected to each other
- High-speed interconnect between compute nodes
- Example: LUMI
  - 2064 CPU nodes<br>(~260,000 CPU cores in total)
  - 2978 GPU nodes<br>(~12,000 MI250X GPUs in total)
</div>
<div class=column>
<center>
![](img/lumi.jpg){.center width=100%}
EuroHPC JU LUMI (HPE Cray EX) <br>at CSC (2021–)
</center>
</div>

# Lumi

 ![](img/lumi.png){.center width=50%}



# GPUs are becoming the norm

<center>
![](img/top500_nov21.svg){.center width=65%}

- Top500 supercomputers grouped by the accelator type (Nov 2021 list)
</center>

# GPUs are becoming the norm

<center>
![](img/top500_nov22.svg){.center width=65%}

- Top500 supercomputers grouped by the accelator type (Nov 2022 list)
</center>

# GPUs are becoming the norm

<center>
![](img/top500_nov24.svg){.center width=65%}

- Top500 supercomputers grouped by the accelator type (Nov 2024 list)
</center>

# Accelerators/GPUs {.section}

# High Performance Computing through the ages

<div class="column">

![](img/top_500.png){.center width=63%}
</div>

<div class="column"> 
![](img/microprocessor-trend-data.png){.center width=100%}
</div>


# Accelerators/GPUs

- Specialized parallel hardware for compute-intensive operations
    - Co-processors for traditional CPUs
    - Based on highly parallel architectures
    - Graphics processing units (GPU) have been the most common
      accelerators during the last few years
- Promises
    - Very high performance per node
    - More FLOPS/Watt
- Usually major rewrites of programs required


# Challenges in using Accelerators/GPUs

**Applicability**: Is your algorithm suitable for GPU?

**Programmability**: Is the programming effort acceptable?

**Portability**: Rapidly evolving ecosystem and incompatibilities between vendors.

**Availability**: Can you access a (large scale) system with GPUs?

**Scalability**: Can you scale the GPU software efficiently to several nodes?

# CPU vs. GPU


#  Heterogeneous Programming Model

- GPUs are co-processors to the CPU
- CPU controls the work flow:
  - *offloads* computations to GPU by launching *kernels*
  - allocates and deallocates the memory on GPUs
  - handles the data transfers between CPU and GPUs
- CPU and GPU can work concurrently
   - kernel launches are normally asynchronous


# Using GPUs

<div class="column">

1. Use existing GPU applications
2. Use accelerated libraries
3. Features in language standards
4. Directive based methods
    - OpenMP, OpenACC
5. High-level GPU programming
    - **C++ stdpar**, **Kokkos**, **SYCL**, ...
6. Use direct GPU programming
    - CUDA, HIP, ...
</div>
<div class="column">
**Easier, more limited**

![](img/arrow.png){width=16% }

**More difficult, more opportunities**

</div>



# GPUs @ CSC

- **Puhti-AI**: 80 nodes, total peak performance of 2.7 Petaflops
    - Four Nvidia V100 GPUs, two 20-core Intel Xeon processors, 3.2 TB fast local storage, network connectivity of 200Gbps aggregate bandwidth  
- **Mahti-AI**: 24 nodes, total peak performance of 2. Petaflops
    - Four Nvidia A100 GPUs, two 64-core AMD Epyc processors, 3.8 TB fast local storage,  network connectivity of 200Gbps aggregate bandwidth   
- **LUMI-G**: 2978 nodes, total peak performance of 500 Petaflops
    - Four AMD MI250X GPUs, one 64-core AMD Epyc processor, network connectivity of 800Gbps aggregate bandwidth


# Summary

- GPUs can provide significant speed-up for many applications
    - High amount of parallelism required for efficient utilization of GPUs
- GPUs are co-processors to CPUs
   - CPU offloads computations to GPUs and manages memory
- Programming models for GPUs
    - Directive based methods: OpenACC, OpenMP
    - Frameworks: Kokkos, SYCL
    - C++ language extensions: CUDA, HIP
