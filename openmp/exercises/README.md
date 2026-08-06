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

### Device functions

- [Heat equation: Restructuring code](09-heat-kernels/)

### Unified shared memory

- [Using unified shared memory in axpy](10-axpy-usm/)


## Friday

### Interoperability with libraries

- [Examining pointers](11-interop/)
- [Call axpy using GPU libraries](12-axpy-blas/)

### Using multiple GPUs

- [Send and receive messages between GPUs](13-mpi-send-recv/)
- [Heat equation: Using multiple GPUs](14-heat-multi-gpu/)
