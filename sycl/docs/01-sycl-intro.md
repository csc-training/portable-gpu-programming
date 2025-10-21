---
title:    Introduction to SYCL
event: Portable GPU Programming
date:     October 2025
lang:     en
---

# Introduction to SYCL{.section}

# What is SYCL?

 - C++ abstraction layer that can target various heterogeneous platforms in a single application
 - single source, high-level programming model
 - open source, royalty-free
 - developed by the Khronos Group 
    - 1.2 (2014), final (2015) revise 1.2.1 (2017)
    - 2.2 (2016), never finalized, C++14 and OpenCL 2.2
    - 2020 (2021), revision 9 (2024), C++17 and OpenCL 3.0
 - focused on 3P (Productivity, Portability, Performance)


# Productivity, Portability, Performance

 - **Productivity**: uses generic programming with templates and generic lambda functions.


 - **Portability**: it is a standard


 - **Performance**: implementations aim to optimize SYCL for specific hardware platforms

# SYCL Implementation


  - specific  adaptation of the SYCL programming model
    - **compilers**:  translate the SYCL code into machine code that can run on various hardware accelerators
    - **runtime library**: manages the execution of SYCL applications, handling  memory management, task scheduling, and synchronization across different devices
    - **backend support**: interface for various backends such as OpenCL, CUDA, HIP,  Level Zero, OpenMP
    - **standard template library**: interface for accesing functionalities and optimizations specific to SYCL
    - **development tools**: debuggers, profilers, etc.


# SYCL Ecosystem

![https://www.khronos.org/blog/sycl-2020-what-do-you-need-to-know](img/2020-blog-sycl-03.jpg){.center width=75%}


# SYCL Implementations

- **Mahti and LUMI**
   - **AdaptiveCpp** (former OpenSYCL, hipSYCL):
     - CPUs (via OpenMP)
     - Intel GPUs (via Level Zero) (**very limited support!**)
     - Nvidia GPUs (via CUDA), AMD GPUs (via ROCM)

- **Intel GPUs**
  - **Intel One API** ~~+ CodePlay Plug-ins for Nvidia and AMD~~:
    - CPUs, Intel GPUs, Intel FPGAs (via OpenCL or Level Zero)
    - ~~Nvidia GPUs (via CUDA), AMD GPUs (via ROCM)~~


# Summary

 - single source, high-level, standard C++  programming model 
 - can target various heterogenous platforms in a single application
     - CPUs, GPUs (Intel, ~~Nvidia, AMD~~), FPGAs, ...
 - 3P: **Portability**, **Productivity**, **Performance**
 - SYCL implementations, specific adaptions 
 - SYCL on Mahti and LUMI: AdaptiveCpp
