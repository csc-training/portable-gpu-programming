<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# OpenMP exercises

## Tuesday

### Getting started

- [Hello world](01-hello/)
- [First GPU kernel: axpy](02-axpy/)

### Controlling data movement

- [Data regions in the axpy code](03-axpy-data/)
- [Heat equation](04-heat/)

### Reduction

- [Parallel sum](05-reduction-sum/)
- [Heat equation: Monitoring average temperature](06-heat-reduction/)


## Wednesday

### Asynchronous kernel execution

- [Heat equation: Asynchronous GPU execution](07-heat-async/)
- [Heat equation: Overlapping I/O with GPU execution](08-heat-io/)

### Unified shared memory

- [Using unified shared memory in axpy](09-axpy-usm/)

### Device functions

- [Declaring device functions for OpenMP offload](10-axpy-device-functions/)


## Friday

### Interoperability with CUDA/HIP and libraries

- [Examining pointers](11-interop/)
- [Calculate axpy using GPU BLAS libraries](12-axpy-blas/)
- [Heat equation: Integrate custom CUDA/HIP kernels](13-heat-kernels/)

### Using multiple GPUs

- [Send and receive messages between GPUs](14-mpi-send-recv/)
- [Heat equation: Using multiple GPUs](15-heat-multi-gpu/)
