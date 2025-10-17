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
    std::vector<int> hx(N),h_y(N);
    {
      buffer<int, 1> x_buf(hx.data(), sycl::range<1>(N)); 
      buffer<int, 1> y_buf(h_y.data(), sycl::range<1>(N)); 
      // Launch kernel 1 Initialize X
      q.submit([&](sycl::handler &cgh) {
        accessor x(x_buf, cgh, read_only);
        h.parallel_for(N, [=](sycl::id<1> i) {
            x[i] = 1;
        });
      });
      // Launch kernel 2: Initialize Y
      q.submit([&](sycl::handler &cgh) {
        accessor y(y_buf, cgh, read_write);
        h.parallel_for(N, [=](sycl::id<1> i) {
            y[i] = 2; 
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
        auto x = accessor{x_buf, cgh, read};
        auto y = accessor{y_buf, cgh, read_write};
        h.parallel_for(N, [=](sycl::id<1> i) {
           y[i] += a * y[i]; // Y = Y + a * X
        });
      });
      // Use host_accessor to read back the results from Ybuff
      host_accessor result{y_buf, read_only)}; // Read back data after kernel execution
      for (int i = 0; i < N; i++) {
        assert(result[i] == 5);}
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
    int *d_x = malloc_device<int>(N, q);
    int *d_y = malloc_device<int>(N, q);

    // Initialize X on the device using a kernel
    event init_X = q.submit([&](handler &cgh) {
        cgh.parallel_for(N, [=](id<1> i) {
            d_x[i] = 1; 
        });
    });

    // Initialize Y on the device using a separate kernel
    event init_Y = q.submit([&](handler &cgh) {
        cgh.parallel_for(N, [=](id<1> i) {
            d_y[i] = 2; 
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
        cgh.depends_on({init_x, init_y}); // Ensure x,y are initialized first
        cgh.parallel_for(N, [=](id<1> i) {
            y[i] += a * x[i]; // Perform Y = Y + a * X
        });
    });

    // Copy results back to host, depending on add_event completion
    int *h_y = new int[N];
    q.submit([&](handler &cgh) {
        cgh.depends_on(add_event); // Ensure add_event (final computation) is done first
        cgh.memcpy(h_y, d_y, N * sizeof(int)); // Copy results back to host
    }).wait(); // Wait for the memcpy to finish

    // Clean up
    delete[] h_y;
    free(d_x, q);
    free(d_y, q);
``` 
</small>

</div>

# Synchronization with Host

 - `q.wait();` pauses the execution until all operations in a queue completed
    - coarse synchonizations, not beneficial if only the results of some kernels are needed at the moment
 - use buffers features:
    - put the buffers in a scope
      - when a buffer goes out of scope program  wait for all actions that use it to complete
    - `host_accessor` will hold the execution until the actions are completed and the data is available to the host
 - synchronize on events,  `e.wait();` or `event::wait({e1, e2});`
    - fine control

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
