<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Introduction to GPUs
event: Portable GPU Programming 2026
lang:  en
---

# Outline

- What is a GPU and how does it differ from a CPU?
- What problems are a good fit for GPUs?
- General principles in programming GPUs


# What is a GPU? {.section}

# GPUs and accelerators

- Specialized parallel hardware for compute-intensive operations
  - Co-processors for CPUs, not replacement of CPUs
  - Based on highly parallel hardware architectures
  - Graphics processing units (GPU) have been the most common accelerators in the past years
- Promises
  - Very high performance per node
  - More FLOPS/Watt
- Usually major rewrites of programs required


# Different hardware design philosophies

<div class="column">

**CPU** 🐆<br> *How fast can a single worker finish<br> a single task?* <br>

- General purpose, good for serial processing
- Few powerful cores with large cache and complex control logic
- Low latency per thread
- Low memory bandwidth
- Great for task parallelism (MIMD)


</div>

<div class="column">

**GPU** 🐜🐜🐜<br> *How fast can all the workers finish<br> all the tasks together?*<br>

- Highly specialized for parallel processing
- Thousands of simpler cores for arithmetic computations
- High latency per thread
- High memory bandwidth
- Great for data parallelism (SIMD)

# GPU architecture implications: Parallelism requirement

- Thousands of cores requires a lot of computation work that can be executed in parallel
- A serial algorithm uses only a tiny fraction of GPU capacity
- Not all problems parallelize easily

# GPU architecture implications: Memory bandwidth

- Feeding enough data for compute cores requires high memory bandwidth

:::::: {.columns}
::: {.column width="50%"}
![](img/cpu-gpu-interconnect.png){.center width=100%}
:::
::: {.column width="50%"}
<small>
<center>
**Peak theoretical bandwidth**

|      | CPU-GPU bus | GPU memory |
|------|------------:|--------------:|
| Mahti: A100  | 32+32 GB/s | 1555 GB/s|
| LUMI: MI250X | 36+36 GB/s | 1600 GB/s|
| Roihu: GH200 | 450+450 GB/s | 3928 GB/s |

</center>
</small>
:::
::::::


# GPU: High latency, high throughput

- Calculating a single value is slow compared to CPU (i.e. latency is high)
- With the same latency you get many values at the same time<br> &rarr; throughput is high
- CPUs are optimized for low latency, GPUs for high throughput

:::::: {.columns}
::: {.column width="80%"}
![](img/low-latency-high-throughput.png){.center width=100%}
:::
::: {.column width="20%"}
  \
  \
  \
Image:<br>J. Lankinen
:::
::::::

# Using GPUs requires algorithmic changes

- Many algorithms need restructuring for efficiency on GPUs

:::::: {.columns}
::: {.column width="50%"}

- CPU: Reduction with a single loop with an accumulator

![](img/reduction-simple.svg){.center width=60%}

:::
::: {.column width="50%"}

- GPU: Hierarchical reduction with parallel execution

![](img/reduction-tree.svg){.center width=60%}
:::
::::::


# Problems that map well to GPUs {.section}

# Problem characteristics: Low coupling & parallelism

:::::: {.columns}
::: {.column width="50%"}
- Problems with low coupling and many independent elements
- Examples
  - For loops with independent iterations
  - Reductions (e.g. sums, max operations) across large arrays
  - Matrix/vector products with many vectors/large matrices

:::
::: {.column width="50%"}
```cpp
for (int i = 0; i < n; i++) {
    y[i] = a * x[i] + y[i];
}
```
![](img/matrix-vector-product.png){.center width=100%}
:::
::::::

# Application examples

- Particle simulations with limited coupling
  - Molecular dynamics with cutoff distances
- Grid-based simulations where cells are updated independently
  - Lattice-Boltzmann methods
- Image processing
  - Rendering 2D/3D scenes (original purpose of GPUs)
- Machine learning and AI with matrix operations
  - Natural language processing
  - Computer vision

# Does your problem benefit from a GPU?

1. Does my problem have many parallelizable work?
2. Do I have a lot of data to crunch over?
3. Can I minimize CPU--GPU data movement?
4. Do I need low latency or high throughput?

# How to approach using GPUs?

1. Is software available? (GROMACS, LAMMPS)
2. Can I use generic libraries? (Thrust, rocBLAS)
4. Do I need portability, ease of development, efficiency, feature support?
5. Low-level API with maximum control or a higher level abstraction?


# Programming GPUs {.section}

# Heterogeneous programming model

- GPU is a **co-processor** for CPU: CPU controls the GPU:
  - CPU *offloads* computations to GPU by launching *kernels*
  - CPU allocates and deallocates the memory on GPUs
  - CPU handles the data transfers between CPU and GPUs
- CPU and GPU can do work concurrently

![](img/async-cpu-gpu.png){.center width=100%}


# Programming languages

- Directive-based approaches: **OpenMP**, OpenACC
- Low-level GPU programming: CUDA or HIP
- Performance portability libraries (C++): **Kokkos**, SYCL, Alpaka, ...
- Deep-learning libraries: PyTorch, TensorFlow, JAX, ...

# GPU execution model

- A central concept: thread
  - GPU threads are very light execution contexts<br>(very different from CPU threads)
- Threads are executed in groups (aka *warps* or *wavefronts*)<br>in the hardware level
  - A warp has 32 threads (A100, GH200) or 64 threads (MI250X)
  - All threads in a warp execute the same instructions but the data is different for each thread (SIMD: Single Instruction, Multiple Data)
- Single-thread execution (e.g., due to branching) means that all the other threads in the warp do nothing useful!

# Thread blocks and grids of blocks

- Threads are grouped in *blocks*
  - Each block is executed by a single hardware unit (compute unit / streaming multiprocessor)
  - Maximum number of threads in a block limited by hardware
  - Good block size is typically a multiple of warp size (e.g., 128 or 256)
- Thread blocks are organized into a *grid*
  - Total number of threads = number of blocks × threads per block
  - In order to hide latencies, the code should execute more blocks than the hardware has compute units

# GPU memory hierarchy

| Type          | Size        | Latency           | Accessibility             |
|---------------|-------------|-------------------|---------------------------|
| CPU global memory | ~1000 GB    | >10k cycles       | host (sometimes device)   |
| GPU global memory | ~100 GB     | 100-1000 cycles   | entire device (grid)      |
| GPU local memory  | ~100 kB     | 1-10 cycles       | compute unit (block)      |
| GPU registers     | ~10 kB      | 1 cycle           | core (thread)     |


# Coalesced memory access

- GPUs accesses global memory in contiguous chunks<br>(e.g., 32, 64, or 128 bytes at a time)
- For best performance, all this loaded data should be used for compute
- When threads in a warp operate on aligned, adjacent elements,
  memory loads and stores are *coalesced* &rarr; all loaded data used

# Performance considerations

- Typical bottlenecks
  - Too many host--device data transfers
  - Too little parallel work for GPUs
  - Unoptimal memory access
  - Unoptimal kernel code
- Do not guess, but use profiling tools to find where the real bottleneck is!


# Summary {.section}

# Summary

- GPUs are **co-processors** for CPUs
  - CPU offloads computations to GPUs and manages its memory
- GPUs are devices optimized for **high throughput for a large amount of data**, not for high speed for a *single* value
  - GPU hardware does compute in inherently parallel fashion
- Many problems map well to the parallel nature of GPUs, but not all
  - Serial algorithms need to be redesigned to parallel algorithms

