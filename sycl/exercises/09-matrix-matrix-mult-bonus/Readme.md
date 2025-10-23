# Memory Optimization II

In the previous [exercise](../07-jacobi/) we saw that having bad data management can have huge impact on the performance. But once we optimized the data movement between CPU and GPU we also need to make sure that it is efficiently access when executing the kernels.

In GPU memory loads from the memory is done in blocks. When gpu thread (or work item) need data from the GPU memory it triggeres a data movement of `32B`, `64B`, or `128B`. However threads are physically locked in sub-groups (called warps in CUDA  or waves in Rocm). In order to maximize performance we need to make sure that all threads in a sub-group execute the same instruction and that the data corresponding to work items are close in the memory. This way instead having  64 B or 128B loaded for each work item, we have only on memory operation for all sub-group. This is called `coalesced` access. The GPUs in general have some caching capabilities, however very limited and it is the job of the programmer to write kernels ensuring coalesced accesses. 

A simple access pattern `c[id]=b[id]+a[id]` is coalesced if `id` is the index of the work item. In cases with complicated access patterns we can try to use `local share memory` to improve the memory operations. In addition to improving  access the access pattern, the local share memory can be used as a programmable cache so that data which is needed by several work items in the same work group can be used without additional memory operations.

Dense matrix operations are example of problems were optimizing  memory accesses is critical for good performance. Let's consider the matrix-matrix multiplication `C=AxB`, with all dimensions equal to `N`. In a naive implementation we would launch a a grid of threads `(N,N)` with each thread processing one element `(i,j)` of C:

```
//# Define size for ND-Range and work-group size
            range<2> global_size(N,N);
            range<2> work_group_size(M,M);

            //# Parallel Compute Matrix Multiplication
            h.parallel_for(nd_range<2>{global_size, work_group_size}, [=](nd_item<2> item){
                const int i = item.get_global_id(0);
                const int j = item.get_global_id(1);
                for (int k = 0; k < N; k++) {
                    matrix_c[i*N+j] +=matrix_a[i*N+k] * matrix_b[k*N+j];
                }
            });
```
We spot several things wrong in this approach. First the construct `matrix_c[i*N+j] += ...` implies that for each value of `k` we have a read and write to the GPU memory. Second all work items with identical `i` index will load from the memory the same data `matrix_a[i*N+k]`, while  all work items with the same `j` load the same data `matrix_b[k*N+j]`. Also the `matrix_a[i*N+k]` access is not coalesced. If we consider the internal coordinate of a work item inside a group, it is between `0`and `M-1`. If `i` is the same for all work-item in the sub-group they are doing one memory operations which loads a block of 64B or 128B, but only need one element from it, while is `i` different the accesses are spaced in memory which results in extra inefficient operations.  Similarly with the `matrix_b[k*N+j]` access. All work-items in a sub-group access the same element, but the memory operations load blocks. There is some limited chaching done automatically by the GPU, but the performance is not guarantied for all applications.

## The Task

In this exercise you need to improve the performance of the kernel only. This can be done gradually. First eliminate the redundant memory accesses by using a local  temporary variable. The new code would look like this:
```

                for (int k = 0; k < N; k++) {
                    temp += matrix_a[i*N+k] * matrix_b[k*N+j];
                }
```

Check the performance change. 

Further improvements can be done. We can define two tiles on the local share memoryy, one for the `matrix_a` and one for `matrix_b`. Each block first loads using coalesced accesses a tile of size `MxM` in  the local share memory. Then the matrix-matrix multiplication is done using this saved tiles.
```
            //# Create local accessors. They use the memory closer to the chip.
            //# In SYCL called local memory. On nvidia and AMD thw so-called shared memory
            local_accessor<float, 2> A_tile(range<2>(M, M), h);
            local_accessor<float, 2> B_tile(range<2>(M, M), h);

            //accessor<float, 2, access::mode::read_write, access::target::local> A_tile(range<2>(M, M), h); //Deprecated
            //accessor<float, 2, access::mode::read_write, access::target::local> B_tile(range<2>(M, M), h); //Deprecated

            //# Parallel Compute Matrix Multiplication
            h.parallel_for(nd_range<2>{global_size, work_group_size}, [=](nd_item<2> item){
                const int i = item.get_global_id(0);
                const int j = item.get_global_id(1);
                const int x = item.get_local_id(0);
                const int y = item.get_local_id(1);
                float temp=0.f;
                int k;
                for (int t = 0; t < N; t+=M) {
                     // save a tile locally for fast access by all threads in a group (block in cuda/hip)
                     A_tile[x][y] = matrix_a[i * N + (t + y)]; 
                     B_tile[x][y] = matrix_b[(t + x) * N + j];
                     item.barrier(access::fence_space::local_space); // barrier within the group
                     for (k = 0; k < M; k++) {
                          temp += A_tile[x][k] * B_tile[k][y];
                     }
                     item.barrier(access::fence_space::local_space); // barrier within the group
                }
```

A more detailed explanation can be found in this [video](https://youtu.be/vyfVDyk7EH0?si=1p0h_FQFgSLS_G3z&t=1051) and [CUDA User's Guide](https://docs.nvidia.com/deeplearning/performance/dl-performance-matrix-multiplication/index.html). 
You can check the performance of the code against the `mkl` library on Intel DevCloud, or against CUDA/HIP Blas libraries on Mahti and LUMI. 

**Note** In many problems the performance dependens on the size of the work group! So for each version of the code different values of `M` need to be tested ! On GPUs the size of the work groups is limited to 1024 so the maximum size for M is 32!
