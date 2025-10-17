---
title:  Further SYCL Features
subtitle: Portable GPU Programming 
date:     October 2025
lang:     en
---

# SYCL Dependencies{.section}

# Task Graph as a Directed Acyclic Graph

![Examples of a linear chain (left) and Y-pattern (right) of dependencies.](img/graphs.svg){.center width=60%}

# Set Dependencies

  - **buffers and accessors**: automatic dependencies based on data and order of submission
  - **in-order** queues: implicit dependence depending on the order of submission
  - **event based**: manual dependencies, most control



# Dependencies via Buffer and Accessors API

<div class="column">
<small>
```cpp
    std::vector<float> Xhost(N),Yhost(N);
    {
      buffer<float, 1> Xbuff(Xhost.data(), sycl::range<1>(N)); 
      buffer<float, 1> Ybuff(Yhost.data(), sycl::range<1>(N)); 
      // Launch kernel 1 Initialize X
      q.submit([&](sycl::handler& h) {
        auto accX = Xbuff.get_access<sycl::access::mode::write>(h);
        h.parallel_for(N, [=](sycl::id<1> i) {
            accX[i] = static_cast<float>(i); // Initialize X = 0, 1, 2, ...
        });
      });
      // Launch kernel 2: Initialize Y
      q.submit([&](sycl::handler& h) {
        auto accY = Ybuff.get_access<sycl::access::mode::write>(h);
        h.parallel_for(N, [=](sycl::id<1> i) {
            accY[i] = static_cast<float>(2 * i); // Initialize Y = 0, 2, 4, 6, ...
        });
      }); 
``` 
</small>

</div>

<div class="column">

<small>
```cpp      
      // Launch kernel 3: Perform Y = Y + a * X
      q.submit([&](sycl::handler& h) {
        auto accX = Xbuff.get_access<sycl::access::mode::read>(h);
        auto accY = Ybuff.get_access<sycl::access::mode::write>(h);
        h.parallel_for(N, [=](sycl::id<1> i) {
            accY[i] += a * accX[i]; // Y = Y + a * X
        });
      });
      // Use host_accessor to read back the results from Ybuff
      host_accessor accY(Ybuff, sycl::read_only); // Read back data after kernel execution
      std::cout << "First few elements of Y after operation:" << std::endl;
      for (size_t i = 0; i < 10; ++i) {
        std::cout << "Y[" << i << "] = " << accY[i] << std::endl;
      }
    }
``` 
</small>

</div>
 - kernel 1 and kernel 2 are independent
 - kernel 3 waits for the completion of kernel 1 and 2 

# Order of Execution in Queues

 - two flavors of queues:
    - **out-of-order**
        - default behaviour
        - a task/kernel can start execution at any time
        - dependencies and order need to be set in other ways
    - **in-order**: 
        - `queue q{property::queue::in_order()};`
        - creates a linear task graph
        - a task/kernel  will start execution only when the preceeding is completed
        - no conncurrent execution

# Event Based Dependencies I
 - most flexible way to force specific order of execution
 - methods on the **handler** class or on the **queue** class return  **event** class objects
      - `event e = q.submit(...)` or `event e = q.parallel_for(...)` 
 - en event or an array of events can  be passed to the **depends_on** method on a handler or to **parallel_for** invocations
      - `cgh.depends_on(e)`  or `q.parallel_for(range { N }, e, [=]...)` 

# Event based dependencies II

<div class="column">
<small>
```cpp
      // Allocate device memory for X and Y
    float *X = malloc_device<float>(N, q);
    float *Y = malloc_device<float>(N, q);

    // Initialize X on the device using a kernel
    event init_X = q.submit([&](handler &cgh) {
        cgh.parallel_for(N, [=](id<1> i) {
            X[i] = static_cast<float>(i); // Initialize X = i
        });
    });

    // Initialize Y on the device using a separate kernel
    event init_Y = q.submit([&](handler &cgh) {
        cgh.parallel_for(N, [=](id<1> i) {
            Y[i] = static_cast<float>(i * 2); // Initialize Y = 2 * i
        });
    });
```
</small>

</div>

<div class="column">

<small>
```cpp

    // Perform Y = Y + a * X on the device after both initializations
    event add_event = q.submit([&](handler &cgh) {
        cgh.depends_on({init_X, init_Y}); // Ensure Y is initialized first
        cgh.parallel_for(N, [=](id<1> i) {
            Y[i] = Y[i] + a * X[i]; // Perform Y = Y + a * X
        });
    });

    // Copy results back to host, depending on add_event completion
    float *host_Y_result = new float[N];
    q.submit([&](handler &cgh) {
        cgh.depends_on(add_event); // Ensure add_event (final computation) is done first
        cgh.memcpy(host_Y_result, Y, N * sizeof(float)); // Copy results back to host
    }).wait(); // Wait for the memcpy to finish

    // Clean up
    delete[] host_Y_result;
    free(X, q);
    free(Y, q);
``` 
</small>

</div>

# Synchronization with Host

 - `q.wait();` pauses the execution until all operations in a queue completed
    - coarse synchonizations, not beneficial if only the results of some kernels are needed at the moment
 - synchronize on events,  `e.wait();` or `event::wait({e1, e2});`
    - fine control
 - use buffers features:
    - `host_accessor` will hold the execution until the actions are completed and the data is available to the host
    - put the buffers in a scope
      - when a buffer goes out of scope program  wait for all actions that use it to complete

# Basic Profiling{.section}

#  Profiling with Events I

 - the queue needs to be initialized for profiling:
    - `queue q{ gpu_selector{}, { property::queue::enable_profiling() } };`
 - submit the work:
    - `auto e = Q.submit([&](handler &cgh){ /* body */});`
 - wait for the task to complete:
    - `e.wait();` (could be also other ways)
 - extract the time:
   - `auto t_submit = e.get_profiling_info<info::event_profiling::command_submit>();`

# Profiling with Events II

 - `get_profiling_info()`  can have different queries:
    - **info::event_profiling::command_submit**: timestamp when command group was submitted to the queue
    - **info::event_profiling::command_start** : timestamp when the command group started executionexecuting 
    - **info::event_profiling::command_end**: timestamp when the command group  finished execution
  - all results are in nanoseconds

# Error Handling {.section}

# Synchronous exceptions vs. Asynchronous exceptions

  - in C++ errors are handled through exceptions:
    - **synchronous exceptions**:
        - thrown immediately when something fails (caught by `try..catch` blocks)
  - SYCL kernels are executed asychronous:
    - **asynchronous exceptions**:
        - caused by a "future" failure 
        - saved into an object 
        - programmer controls when to process

# Processing Asynchronous exceptions

<div class="column">
<small>
```cpp
#include <sycl/sycl.hpp>
using namespace sycl;

// Asynchronous handler function object
auto exception_handler = [] (exception_list exceptions) {
    for (std::exception_ptr const& e : exceptions) {
      try {
        std::rethrow_exception(e);
      } catch(exception const& e) {
        std::cout << "Caught asynchronous SYCL exception:\n"
                  << e.what() << std::endl;
      }
    }
  };
```
</small>

</div>

<div class="column">

<small>
```cpp

  int main() {
  sycl::queue queue(default_selector_v, exception_handler);

  queue.submit([&] (handler& cgh) {
    auto range = nd_range<1>(range<1>(1), range<1>(10));
    cgh.parallel_for(range, [=] (nd_item<1>) {});
  });

  try {
    queue.wait_and_throw();
  } catch (exception const& e) {
    std::cout << "Caught synchronous SYCL exception:\n"
              << e.what() << std::endl;
  }
}
``` 
</small>
</div>

<small>e.g. https://developer.codeplay.com/computecppce/latest/sycl-guide-error-handling</small> 


# Summary
- dependencies
    - using buffer and accessors API
    - using **in-order** queues
    - using events
- basic profiling using events
- error handling
