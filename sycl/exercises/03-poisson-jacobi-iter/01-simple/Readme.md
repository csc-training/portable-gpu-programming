# Solving Poisson Equation with Jacobi Iterations
For this exercise we try to solve:

$$
\nabla^2 u(x, y) = 0
$$

**Structure of the Code**:
  1. define a SYCL  queue (choose device and specify the options)
  1. declare  the variables
  1. initialize the input variables (on CPU or on device using a separate kernels for each array)
  1. transfer the necesary data from CPU to device (not needed when the problem is initialized on the device)
  1. do the `axpy` computation in a separate kernel 
  1. copy data to host to check the results

## I. Naive Jacobi Iterations  with Buffers and Accessors Memory 

Starting from the [skeleton code](j_simple_with_buffer.cpp) implement a solver for the Poisson equation in 2D using Finite Diferences. 
Look for the **//TODO** lines.

### Step 1: Define a Queue
Start by defining a **queue**  and selecting the appropriate device selector. SYCL provides predefined selectors, such as: default, gpu, cpu, accelerator:

- `queue q(default_selector_v);`  or `queue q;` targets the best available device
-  `queue q(cpu_selector_v);` targets the best CPU
-  `queue q(gpu_selector_v);` targets the best GPU
-  `queue q(accelerator_selector_v);` targets the best accelerator
    
Alternatively it is possible to use the procedure from the [previous exercise](../01-info/enumerate_device.cpp). This the recommended when the application needs to detect multiple GPUs and assign devices according to MPI rank or OpenMP thread index.

### Step 2: Create Buffers
Create buffers to encapsulate the data. One buffer is for the present iterations and one for the next one 

```cpp
    buffer<float, 1> u(matrix_u.data(), range<1>(nx*ny)); 
    buffer<float, 1> unew(matrix_unew.data(), range<1>(nx*ny));
```


### Step 3: Create Accessors
Accessors provide access to buffer data and must be created inside command groups on the device.

Two ways to create accessors:

```cpp
   sycl::accessor x_acc{x_buf, h, read_write};
   
```
or  
```cpp
   accessor U(u, h, sycl::read_only);
   accessor UNEW(unew, h, sycl::write_only);
```
**Important**  Use appropriate access modes for your data:
 - **Input Buffers:** Use `sycl::read_only` / `sycl::access::mode::read` to avoid unnecessary device-to-host data transfers.
 - **Output Buffers:** Use `sycl::write_only`/ `sycl::access::mode::write` to avoid unnecessary host-to-device data transfers.
 - **Input/Ouput Buffers:** Use `sycl::read_write` / `sycl::access::mode::read_write`  for data both read and modified during computation.


### Step 4: Submit the Task using Simple Launching
Once accessors are ready, submit the task to the device using .parallel_for():
```cpp
q.submit([&](handler& h) {
      // Create accessors
      
      accessor U(u, h, sycl::read_only);
      accessor UNEW(unew, h, sycl::write_only);

      range<2> global_size(nx,ny);

      h.parallel_for(global_size, [=](id<2> item){
                const int i = item[0];
                const int j = item[1];

                int ind = i * ny + j;
                int ip = (i + 1) * ny + j;
                int im = (i - 1) * ny + j;
                int jp = i * ny + j + 1;
                int jm = i * ny + j - 1;
                if(i>0 && i<nx-1 && j>0 && j< ny-1){
                    UNEW[ind] = factor * (U[ip] - 2.0 * U[ind] + U[im] +
                                 U[jp] - 2.0 * U[ind] + U[jm]);
                }         
            });
});
```
This step is repeated, but in the second step the accesors  are inverted so that we avoid a redundant data copy from **unew** variable to **u**:

```cpp
q.submit([&](handler& h) {
      // Create accessors
      
      accessor U(unew, h, sycl::read_only);
      accessor UNEW(u, h, sycl::write_only);
      ...
```

**Note that a 2D grid is created and we can associate the `x` and `y` with the thread/work-item indeces `i` and `j`, however the data itself is flattened. As a bonus exercise one can try to use 2D buffers and accessors.**

## II. Measure time using events
Basic profiling can be done by measuring the time spent in kernels and compare it to the total time spent doing the Jacobi Iterations (which include data transfers and other overheads). 
Start from the [solution](j_simple_with_buffer.cpp) of the previuos task. 
### Step I: Modify the Queue
Change the queue to allow for profiling. This is done by defining a SYCL list of properties which is used to initialized the queue in addition to the device selector. 
```
//# Define queue with default device for offloading computation
    sycl::property_list q_prof{property::queue::enable_profiling{}}; // enable profiling 
    queue q{default_selector_v,q_prof}; // selects automatically the best device available
```
### Step II: Add Events
Kernel launches return events. Both calls in one iteration are modified such as:
```
auto e = q.submit(...);
...
```
Time is measured when the kernel execution is completed:
```
kernel_duration += (e.get_profiling_info<info::event_profiling::command_end>() - e.get_profiling_info<info::event_profiling::command_start>());
```
The results are in nanoseconds!

This timing only reflects the time spent in performing calculations on the device. If proper synchornization between host and device is done the total duration time can be measured using a library such as `std::chrono`.

Before the Jacobi loop the time is recorded:
```
auto start = std::chrono::high_resolution_clock::now().time_since_epoch().count();
```
The total duration in nanoseconds is obtained by calling:
```
auto duration = std::chrono::high_resolution_clock::now().time_since_epoch().count() - start;
```

## III. Global Memory Optimizations

GPUs are a specialized parallel hardware for floating point operations. The high performance of GPUs comes from masive paralellism by using many "small" gpu cores to perform the same operation on different elements of the data. It is **critical** to keep these gpu cores occupied at all time and provide them enough data to be processed. 

For small problem sizes a GPU  application  can run only on the GPU. The CPU would initialized the data, offload  all work to the GPU, and then at the end collect the data for further analysis. In this case there is data transfer between CPU and GPU only at the beginning and at the end.  However is most applications today can not fit into the memory of a GPU, and many cases not even in a node.  It is extremely important to be aware of how the data is moved between CPU and GPU and minimize these transfers. 

In order to exemplify the importance of this, we consider the simple in which we try to solve a problem by getting a series of solutions, `S[n+1]=f(S[n])` in many steps until the convergence criteria, `|S[n+1]-S[n]| < tol` is reached. 

In the first implementation of  [Jacobi iterations](j_simple_with_buffer.cpp) the focus fwas  elegance and productivity. The application initializes the data on the CPU and then uses buffers to offload the operations to the GPUs. In the application we measure the total time to perform a specific number iterations via C++ chronos library and also the effective time spent in actual computation on the GPU using `sycl::event`.
When program was executed on Mahti with  system size of 16000x16000:
```
./j_simple_buffer -n 16000
```
The application reported a quite small time spent in executing kernels about 0.369 s. However the total time spent to execute all iterations was around 62 s.
```
Offload Device        : NVIDIA A100-SXM4-40GB
max_work_group_size   : 1024
Configuration         : MATRIX_SIZE= 16000x16000
 [0][0] = 8000
Warm up the device  
Kernel Execution Time : 0.368924 seconds
Compute Duration      : 62.1323 seconds
```

In this case we suspect that having the buffers created and destroyed every time step results in data being transfered between CPU and GPU. More information can be obtained using a performance analys tool. Since this is a code running on nvidia GPUs, using cuda as backend we can use the cuda toolkit performance analysis tools included. We can get a lots of info by using [`nsys`](https://docs.csc.fi/computing/nsys/).

```
nsys profile -t nvtx,cuda -o results --stats=true --force-overwrite true ./j_simple_buffer -n 16000
```
From the output we selected only some statistics related to the memory movements:

```
CUDA Memory Operation Statistics (by time):

 Time(%)  Total Time (ns)  Count  Average (ns)   Minimum (ns)  Maximum (ns)  StdDev (ns)       Operation     
 -------  ---------------  -----  -------------  ------------  ------------  ------------  ------------------
    64.3   39,958,457,360    404   98,907,072.7    80,076,865   141,251,921  19,554,201.0  [CUDA memcpy HtoD]
    35.7   22,211,682,360    202  109,958,823.6   107,057,217   123,094,875   2,242,793.1  [CUDA memcpy DtoH]



CUDA Memory Operation Statistics (by size):

 Total (MB)   Count  Average (MB)  Minimum (MB)  Maximum (MB)  StdDev (MB)      Operation     
 -----------  -----  ------------  ------------  ------------  -----------  ------------------
 413,696.000    404     1,024.000     1,024.000     1,024.000        0.000  [CUDA memcpy HtoD]
 206,848.000    202     1,024.000     1,024.000     1,024.000        0.000  [CUDA memcpy DtoH]
```
We note in the upper table that a lot of time (around 62 s) is spent executing cuda memory copy operations, while the lower panel we note that we had 606 memory operations with more more than 600 GB of data moved around. This is 15x more than the whole memory of the A100 GPU. We can conclude that whole data was moved every iteration. 


The exercise is to reduce this data movement. The [solution](solution/) shows a way to solve this problem using unified shared memory (USM) or with buffers and accessors API. But you can try to experiment with moving the buffer declaration outside of the loop over iterations. Use the application timings and also the profilers to get the needed information. You can use Mahti or LUMI for this. 

On LUMI `rocm` is used as a backend. We can use `rocprof` to obtained similar information to the one given by `nsys` using:
```
rocprof --stats --hip-trace --hsa-trace ./j_simple_buffer -n 16000
```
The `rocprof` results are saved in a set of files `resutls.<...>.csv` 
