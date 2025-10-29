<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

---
title: Unified shared memory
event: Portable GPU Programming 2025
lang:  en
---

# Outline

- Unified shared memory in OpenMP offload

# Unified shared memory {.section}

# Unified shared memory

- Modern GPUs support unified virtual addressing so that a single pointer can be used to access memory
  regardless of whether it resides on CPU or GPU
- At the hardware level, this can be implemented in many ways:
  - Physically same memory for CPU and GPU
  - Page migration between CPU and GPU memories
  - Zero-copy data transfer through the bus connecting CPU and GPU memories

# Unified shared memory in OpenMP offload

- OpenMP 5.0 introduced a `requires` construct so that a program can declare it assumes unified shared memory (USM) between host and device
  - Execution of the program should fail if the device does not support the feature

<div class="column">
```cpp
#pragma omp requires unified_shared_memory
```
</div>
<div class="column">
```fortranfree
!$omp requires unified_shared_memory
```
</div>

- When using USM, `data` directives or `map` clauses are not needed


# Unified shared memory in practice

- Currently, performance varies greatly depending on the compiler and the runtime
- See exercises

# Summary

- Unified shared memory is possible with OpenMP offload
