---
title:    SYCL Queues, Command Groups, Kernels
subtitle: Portable GPU Programming
date:     October 2025
lang:     en
---

# Queues, Command Groups, Kernels{.section}

# SYCL Programming Model 

 - Program runs on the CPU (host)
 - Host initializes the devices, allocates the memory, and stages the host-device transfers
    - **Note!** CPU can also be a device
 - Host launches the parallel code (kernel) ito be executed on a device by several threads
 - Code is written from the point of view of a single thread
    - each thread has a unique ID

# Device Discovery

 - it is important to know which devices are available
 - SYCL provides methods for querying:
    - `platform::get_platforms()` gives a list of available platforms
    - `.get_devices()` gives a list of present devices in a specific platform
    - `.get_info<info::device:: <property> >()` gives info about specific property
        - can be applied to various classes
        - wide `sycl::info` namespace 

# SYCL Queues{.section}

# SYCL Queues

 - bridge between the host and the target device (**only one**)
 - associated with a SYCL device and a SYCL context
 - SYCL class responsible for submitting commands
 - enable asynchronous execution
 - has an error-handling mechanism via an optional `exception_handler`
 - are **out-of-order** (default) or **in-order** (`{property::queue::in_order()}`)
 - encapsulates operations (e.g., kernel execution or memory transfers) using **command groups**

# Choosing the Device

  - `queue q();` targets the best device
  - Pre-configured classes of devices:
    - `queue q(default_selector_v);` targets the best device 
    - `queue q(cpu_selector_v);` targets the best CPU
    - `queue q(gpu_selector_v);` targets the best GPU
    - `queue q(accelerator_selector_v);` targets the best accelerator

# Custom Selector

<small>
```cpp
using namespace sycl;
class custom_selector : public device_selector
{
public:
  int operator()(const device &dev) const override
  {
    int score = -1;
    if (dev.is_gpu()) {
      auto vendor = dev.get_info<info::device::vendor>();
      if (vendor.find("NVIDIA") != std::string::npos) score += 75;
      if (vendor.find("Intel") != std::string::npos) score += 50;
      if (vendor.find("AMD") != std::string::npos) score += 100;
    }
    if (dev.is_host()) score += 25; // Give host device points so it is used if no GPU is available.

    return score;
  }
};
``` 
```cpp
auto Q = queue { custom_selector {} };

  std::cout << "we are running on: "
            << Q.get_device().get_info<info::device::vendor>() << " "
            << Q.get_device().get_info<info::device::name>() << std::endl;
```
</small>

# Explicit Way
 - using `get_platforms()` and/or `get_devices` 
```cpp
  std::cout << "\tChecking for GPUs\n" << std::endl;

  auto gpu_devices= sycl::device::get_devices(sycl::info::device_type::gpu);
  auto n_gpus=size( gpu_devices );

  std::cout << "\t\t There are "<< n_gpus << " GPUs\n"<< std::endl;
  if(n_gpus>0){
    queue q{gpu_devices[my_rank]};
  }
  else{
    std::cout << "\t\t There are no GPUs found \n Existing"<< std::endl;
    exit(1);
  }
``` 


# Queue Class Member Functions 

  - **Enqeue work**: `submit()`, `parallel_for()`, `single_task()`
  - **Memory Operations**: `memcpy()` , `fill()`, `copy()`, `memset()`
  - **Utilities**: `is_empty()`,  `get_device()`, `get_context()`
  - **Synchronizations**: `wait()`, `wait_and_throw()`

# Command Groups{.section}

# Command Groups

 - created via `.submit()` member
 - containers for operations to be executed 
 - give more control over executions than:
    - `q.parallel_for(N, [=](id<1> i) { y[i] += a * x[i];});`
 - can have dependencies for ensuring desired order
 - are executed *asynchronous* within specific **context** and **queue**
<small>
```cpp  
  q.submit([&](handler &cgh) {
    auto x = x_buf.get_access<access::mode::read>(h);        
    auto y = y_buf.get_access<access::mode::read_write>(h);  

    h.parallel_for(N, [=](id<1> i) {
      y[i] += a * x[i];
    });
  });
```
</small>

#  Kernels{.section} 

# Kernels
 - code to be executed in parallel
 - written from the point of view of a work-item (gpu thread)
    - each instance gets a unique `id` using the work-item index

<div class="column">
 - lambda expressions
```cpp
    [=](id<1> i) {
      y[i] += a * x[i];
    }
```
</div>

<div class="column">
 - function object (functors)
 <small>
```cpp 
class AXPYFunctor {
public:
  AXPYFunctor(float a, accessor<T> x, accessor<T> y): a(a), x(x),
                                                      y(y) {}

  void operator()(id<1> i) {
    y[i] += a * x[i];
  }

private:
  float a;
  accessor<T> x; 
  accessor<T> y;
};
```
</small>



#  Launching Kernels{.section}

# Grid of Work-Items

<div class="column">


![](img/Grid_threads.png){.center width=37%}

<div align="center"><small>A grid of work-groups executing the same **kernel**</small></div>

</div>

<div class="column">
![](img/mi100-architecture.png){.center width=53%}

<div align="center"><small>AMD Instinct MI100 architecture (source: AMD)</small></div>
</div>

 - a grid of work-items is created on a specific device to perform the work. 
 - each work-item executes the same kernel
 - each work-item typically processes different elements of the data. 
 - there is no global synchronization or data exchange.

# Basic Parallel Launch with `parallel_for`

<div class="column">

 - **range** class to prescribe the span off iterations 
 - **id** class to index an instance of a kernel
 - **item** class gives additional functions 

</div>

<div class="column">

```cpp
cgh.parallel_for(range<1>(N), [=](id<1> idx){
  y[idx] += a * x[idx];
});
``` 

```cpp
cgh.parallel_for(range<1>(N), [=](item<1> item){
  auto idx = item.get_id();
  auto R = item.get_range();
  y[idx] += a * x[idx];
});
```

</div>

 - runtime choose how to group the work-items
 - supports 1D, 2D, and 3D-grids
 - no control over the size of groups, no locality within kernels 


# Parallel launch with **nd-range** I

![](img/ndrange.jpg){.center width=100%}

<small>https://link.springer.com/book/10.1007/978-1-4842-9691-2</small>

# Parallel launch with **nd-range** II

 - enables low level performance tuning 
 - **nd_range** sets the global range and the local range 
 - iteration space is divided into work-groups
 - work-items within a work-group are scheduled on a single compute unit
 - **nd_item** enables to querying for work-group range and index.

```cpp
cgh.parallel_for(nd_range<1>(range<1>(N),range<1>(64)), [=](nd_item<1> item){
  auto idx = item.get_global_id();
  auto local_id = item.get_local_id();
  y[idx] += a * x[idx];
});
```

# Parallel launch with **nd-range** III
 - extra functionalities
    - each work-group has work-group *local memory*
        - faster to access than global memory
        - can be used as programmable cache
    - group-level *barriers* and *fences* to synchronize work-items within a group
        - *barriers* force all work-items to reach a speciffic point before continuing
        - *fences* ensures writes are visible to all work-items before proceeding
    - group-level collectives, for communication, e.g. broadcasting, or computation, e.g. scans
        - useful for reductions at group-level
 

# Summary

 - **queues** are bridges between host and devices
 - each queue maps to one device
 - work is enqued by submitting **command groups**
    - give lots of flexibility
 - parallel code (kernel)  is submitted as a lambda function or as a function operator
 - two methods to express the parallelism
    - basic launching
    - via **nd-range**

