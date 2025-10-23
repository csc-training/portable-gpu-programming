## Message exchange with Kokkos


This exercise should be done only in LUMI as course reservation in Mahti allows only single GPU
runs.

Port the [exchange.cpp](exchange.cpp) code to Kokkos, *i.e.* use Kokkos Views for
message and buffer data structures.

Note that in order to use GPU aware MPI (if Views are on HIP memory space),
you should set the following environment variable before compiling:
```
export HIPCC_LINK_FLAGS_APPEND=$(CC --cray-print-opts=libs)
```

Run the code with two MPI tasks, and set the following environment variable when using GPU
aware MPI:
```
export MPICH_GPU_SUPPORT_ENABLED=1
```
