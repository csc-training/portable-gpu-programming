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


### Step 4: Submit the Task using `nd_range` Launching
Once accessors are ready, submit the task to the device using .parallel_for():
```cpp
q.submit([&](handler& h) {
      // Create accessors
      
      accessor U(u, h, sycl::read_only);
      accessor UNEW(unew, h, sycl::write_only);

      range<2> global_size(nx,ny);
      range<2> work_group_size(M,M);

      h.parallel_for(nd_range<2>{global_size, work_group_size}, [=](nd_item<2> item){
                const int i = item.get_global_id(0);
                const int j = item.get_global_id(1);
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

      range<2> global_size(nx,ny);
      range<2> work_group_size(M,M);

      h.parallel_for(nd_range<2>{global_size, work_group_size}, [=](nd_item<2> item){
                const int i = item.get_global_id(0);
                const int j = item.get_global_id(1);
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

**Note that a 2D grid is created and we can associate the `x` and `y` with the thread/work-item indeces `i` and `j`, however the data itself is flattened. As a bonus exercise one can try to use 2D buffers and accessors.**

## II. Measure time using events
Basic profiling can be done by measuring the time spent in kernels and compare it to the total time spent doing the Jacobi Iterations (which include data transfers and other overheads). 
Start from the [solution](j_simple_with_buffer.cpp) of the previuos task. 
### Step I. Modify the Queue
Change the queue to allow for profiling. This is done by defining a SYCL list of properties which is used to initialized the queue in addition to the device selector. 
```
//# Define queue with default device for offloading computation
    sycl::property_list q_prof{property::queue::enable_profiling{}}; // enable profiling 
    queue q{default_selector_v,q_prof}; // selects automatically the best device available
```
### Step II Add Events
Kernel launchins return events. Both calls in one iteration are modified such as:
```
auto e=e = q.submit(...);
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
The total duration in nanoseconds is obtainied by calling:
```
auto duration = std::chrono::high_resolution_clock::now().time_since_epoch().count() - start;
```
