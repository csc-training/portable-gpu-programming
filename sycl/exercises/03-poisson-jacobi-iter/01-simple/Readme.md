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
Create buffers to encapsulate the data. For a one-dimensional array of integers of length `N`, with pointer `P`, a buffer can be constructed as follows:

```cpp
    sycl::buffer<int, 1> x_buf(P, sycl::range<1>(N));
```
Use the appropriate data type. 


### Step 3: Create Accessors
Accessors provide access to buffer data and must be created inside command groups on the device.

Two ways to create accessors:

```cpp
   sycl::accessor x_acc{x_buf, h, read_write};
```
or  
```cpp
   auto a_acc = sycl::accessor{a_buf, h, sycl::read_write};
```
**Important**  Use appropriate access modes for your data:
 - **Input Buffers:** Use `sycl::read_only` / `sycl::access::mode::read` to avoid unnecessary device-to-host data transfers.
 - **Output Buffers:** Use `sycl::write_only`/ `sycl::access::mode::write` to avoid unnecessary host-to-device data transfers.
 - **Input/Ouput Buffers:** Use `sycl::read_write` / `sycl::access::mode::read_write`  for data both read and modified during computation.


### Step 4: Submit the Task using Basic Submission
Once accessors are ready, submit the task to the device using .parallel_for():
```cpp
q.submit([&](handler& h) {
      // Create accessors
      accessor x_acc(x_buf, h, read_only);
      accessor y_acc(y_buf, h, read_write);

      h.parallel_for(range<1>(N), [=](id<1> i) {
        y_acc[i] = a * x_acc[i] +  y_acc[i];
      });
    });
```
