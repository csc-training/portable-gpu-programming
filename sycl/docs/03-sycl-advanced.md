---
title:    Advanced SYCL
subtitle: Portable GPU Programming
date:     October 2025
lang:     en
---

# Advanced SYCL{.section}

# Buffers and Accessors API 
- **Advantages**
    - automatic host–device data management
    - clear access semantics
    - explicit access modes (**read**, **write**, **read_write**) for safer kernel
    - safe synchronization
    - runtime tracks dependencies and schedules kernels correctly
    - portability
    - structured programs
- **Disadvantages**
    - low performance on host–device data trasnfers
    - management overhead at buffer creation and destruction
    - requires explicit accessor creation with a a specified mode, even for trivial cases
    - unintended synchronizations and accessors deadlocks
    - difficult interoperability
    - challenging debugging
    - no fine-grained control over memory
 
  
# Unified Shared Memory (USM) I

- pointer-based approach to C/CUDA/HIP
- explicit allocation and  freeing of memory
- explicit dependencies
- explicit host-device transfers (unless using managed)
- explicit host-device synchronization 

# Unified Shared Memory II

| Function        | Location	         | Device Accessible
------------------+--------------------+--------------------
| malloc_device	  | Device 	           | Yes                 
| malloc_shared	  | Dynamic migration  | Yes                 
| malloc_host	    | Host  	           | Device can read     

# `malloc_device`  

<small>
```cpp
  std::vector<int> y(N, 1);

  // Allocate device memory
  int* d_y = malloc_device<int>(N, q); 
  // Copy data from host to device
  q.memcpy(d_y, y.data(), N * sizeof(int)).wait(); 

  q.submit([&](handler& cgh) {
    cgh.parallel_for(range<1>(N), [=](sid<1> id) {
      d_y[id] += 1;
    });
  }).wait();
  // Copy results back to host
  q.memcpy(y.data(), d_y, N * sizeof(int)).wait();
  
  // Verify the results
  for (int i = 0; i < N; i++) {
    assert(y[i] == 2);
  }

  // Free the device memory
  sycl::free(d_y, q);
```
</small>


# `malloc_shared`

<small>
```cpp
    int* y = malloc_shared<int>(N, q);
    // Initialize from host
    for (int i = 0; i < N; i++) {
        y[i] = 1;
    }

    q.submit([&](handler& cgh) {
        cgh.parallel_for(range<1>(N), [=](id<1> idx) {
            y[idx] += 1;
        });
    }).wait();

    // No memcpy needed — host can read directly
    for (int i = 0; i < N; i++) {
        assert(y[i] == 2);
        std::cout << "y[" << i << "] = " << y[i] << "\n";
    }

    // Free the shared memory
    sycl::free(y, q);
```
</small>


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
