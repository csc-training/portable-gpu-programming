---
title:    SYCL Fundamentals
subtitle: Portable GPU Programming
date:     October 2025
lang:     en
---

# SYCL Fundamentals{.section}

# SYCL Programming Model 

 - Program runs on one of the cores the CPU (host)
 - Host initializes the devices, allocates the memory, and stages the host-device transfers
    - **Note!** CPU can also be a device
 - Host launches the parallel code (kernel) to be executed on a device by many threads
 - Code is written from the point of view of a single thread
    - each thread has a unique ID

# Device Discovery

 - it is important to know which devices are available
 - SYCL provides methods for querying:
    - `device::get_devices(sycl::info::device_type::<type>)` get a list off all devices
        - `<type>` can be **all**, **cpu**, **gpu**
    - `platform::get_platforms()` gives a list of available platforms
    - `.get_devices()` gives a list of present devices in a specific platform
    - `.get_info<info::device:: <property> >()` gives info about specific property
        - can be applied to various classes
        - wide `sycl::info` namespace 

# SYCL Queues

 - bridge between the host and the target device (**only one**)
 - associated with a SYCL device and a SYCL context
 - SYCL class responsible for submitting commands
 - enable asynchronous execution
 - has an error-handling mechanism via an optional `exception_handler`
 - are **out-of-order** (default) or **in-order** (`{property::queue::in_order()}`)
 - encapsulates operations (e.g., kernel execution or memory transfers) using **command groups**


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

# Choosing the Device

  - `queue q();` targets the best device
  - Pre-configured classes of devices:
    - `queue q(default_selector_v);` targets the best device 
    - `queue q(cpu_selector_v);` targets the best CPU
    - `queue q(gpu_selector_v);` targets the best GPU
    - `queue q(accelerator_selector_v);` targets the best accelerator
  - User defined custom selector:
    - `queue q(custom_selector{});` targets the accelerator based on users' rules

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
auto q = queue { custom_selector {} };

  std::cout << "we are running on: "
            << q.get_device().get_info<info::device::vendor>() << " "
            << q.get_device().get_info<info::device::name>() << std::endl;
```
</small>

# Queue Class Member Functions 

  - **Enqeue work**: `submit()`, `parallel_for()`, `single_task()`
  - **Memory Operations**: `memcpy()` , `fill()`, `copy()`, `memset()`
  - **Utilities**: `is_empty()`,  `get_device()`, `get_context()`
  - **Synchronizations**: `wait()`, `wait_and_throw()`


# Command Groups

 - created via `.submit()` member
 - containers for operations to be executed 
 - can have dependencies for ensuring desired order
 - are executed *asynchronous* within specific **context** and **queue**
<small>
```cpp  
  q.submit([&](handler &cgh) {
  /* Command group function */ 
  });
  // ...
```
</small>


# SYCL Memory Models

 - three memory-management abstractions in the SYCL standard:
     - **buffer and accessor API**: a buffer encapsulate the data and accessors describe how you access that data
     - **images**: similar API to buffer types, but with extra functionality tailored for image processing (will not be discussed here)
     - **unified shared memory**: pointer-based approach, C/CUDA/HIP-like


# Buffers and Accesors I
 -  a **buffer** provides a high level abstract view of the data 
 - support 1-, 2-, or 3-dimensional data
 - dependencies between multiple kernels are implicitly handled
 - does not own the memory, it’s only a *constrained view* into it
 - **accessor** objects are used to access the data on device
 - various access modes, *read_write*, *read_only*, or *write_only*
 - **host_accessor** bjects are used to access the data on host

# Buffers and Accesors II
 
```cpp
  std::vector<int> hy(N, 1);
 {
    // Create buffers for data 
    buffer<int, 1> y_buf(hy.data(), range<1>(N));
    q.submit([&](handler& cgh) {
      accessor y{y_buf, cgh, read_write}; // The encapsulated data is accessed via accessors
      /* Work to be done on the device. Increment each element by 1.*/
    });
    host_accessor result{y_buf}; // host can access data also directly after buffer destruction
    for (int i = 0; i < N; i++) {
      assert(result[i] == 1);
    }
 }
``` 


# Kernels
 - code to be executed in parallel
 - written from the point of view of a work-item (gpu thread)
   
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
template <typename T>
class AXPYFunctor {
public:
  AXPYFunctor(float a, accessor<T> x, accessor<T> y): a(a), x(x),
                                                      y(y) {}

  void operator()(id<1> i) {
    y[i] += a * x[i];
  }

private:
  T a;
  accessor<T> x; 
  accessor<T> y;
};
```
</small>

</div>

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


# AXPY in SYCL with Lambda


<div class="column">
<small>
```cpp
#include <sycl/sycl.hpp>
#include <vector>
#include <iostream>
#include <cassert>
using namespace sycl;

``` 
</small>
</div>


<div class="column">
<small>
```cpp
int main() {
    constexpr size_t N = 8;
    std::vector<int> hx(N, 1), hy(N, 2);
    int a = 3;

    queue q;
    {
        buffer x_buf(hx); buffer y_buf(hy);

        q.submit([&](handler &cgh) {
            auto x = accessor{x_buf, cgh, read};
            auto y = accessor{y_buf, cgh, read_write};

            cgh.parallel_for(range<1>(N), [=](id<1> i) {
                y[i] += a * x[i];
            });
        });
        {
           host_accessor result{y_buf};
           for (int i = 0; i < N; i++) {
            assert(result[i] == 5);}
        }
    }
    // Results are available on host after the buffer is destroyed
}
```  
</small>
</div>

# AXPY in SYCL with Functor


<div class="column">
<small>
```cpp
#include <sycl/sycl.hpp>
#include <vector>
#include <iostream>
#include <cassert>
using namespace sycl;

template <typename T>
class AXPYFunctor {
public:
    // Fixed constructor colon
    AXPYFunctor(T a, accessor<T> x, accessor<T> y) : 
                                 a(a), x(x), y(y) {}

    void operator()(id<1> i) const {
        y[i] += a * x[i];
    }

private:
    T a;
    accessor<T> x;
    accessor<T> y;
};

``` 
</small>
</div>


<div class="column">
<small>
```cpp
int main() {
    constexpr size_t N = 8;
    std::vector<int> hx(N, 1), hy(N, 2);
    int a = 3;

    queue q;
    {
        buffer x_buf(hx); buffer y_buf(hy);

        q.submit([&](handler &cgh) {
            auto x = accessor{x_buf, cgh, read};
            auto y = accessor{y_buf, cgh, read_write};

            AXPYFunctor<int> fun(a, x, y);

            cgh.parallel_for<class AxpyKernel>(range<1>(N), fun);
        });
        {
           host_accessor result{y_buf}; 
           for (int i = 0; i < N; i++) {
            assert(result[i] == 5);}
        }
    }
    // Results are available on host after the buffer is destroyed
}

``` 
</small>
</div>

# Summary

 - **queues** are bridges between host and devices
 - each queue maps to one device
 - work is enqued by submitting **command groups**
 - data manegment via **bufffers** and **accessors**
 - parallel code (kernel)  is submitted as a lambda function or as a functor
 - basic parallelism 

