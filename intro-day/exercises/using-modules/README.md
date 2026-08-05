<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Using modules and compiling

In this exercise we practice how to select compiler with the module system. 

Typically, supercomputers have set of compilers available via the module system. Certain programming paradigms 
(e.g. OpenMP offloading) might be supported only by some compilers, or particular application performs
better with certain compiler (e.g. Cray vs. GCC vs. AMD).

## Tasks: Roihu-GPU

These tasks should be done in Roihu-GPU, so start by logging into `roihu-gpu.csc.fi`

1. Investigate the default set of modules with `module list` (or shorthand `ml`).

2. Compile the [hello.cpp](hello.cpp) code with the GNU compiler `gcc`, run it and investigate the output:

     gcc -o hello_gcc hello.cpp
     ./hello_gcc

   Note: normally you should always run codes via the batch job system.

3. Compile the code with NVIDIA HPC compiler `nvc`. 

   If you try to use `nvc` as such (e.g. `nvc --version`) you will notice that command is not available. 
   Thus, load first the correct module

     module load nvhpc 
     module list

   Proceed then on compiling and running the code:

     nvc -o hello_nvc hello.cpp
     ./hello_nvc

   Note that `gcc` is still available, however, this is the system default with version 11.5.0, and not the 14.3.0
   that is available via module (you can try to run `gcc --version`).

4. Reset the default set of modules with `module reset`, and verify with `module list` and `gcc --version` that
   you have again the default set

## Tasks: LUMI

LUMI has all the modules for both CPUs and GPUs available on a common login node. In Cray environment, compilers are
used via common wrappers (`cc`, `CC`, `ftn`), and the specific underlying compiler is selected via modules.

1. Start by loading the LUMI environment for GPUs, and investigate the default set of modules:

     module load LUMI/25.09 partition/G
     module list

2. Compiler the "hello" code with the (default) Cray compiler, and investigate output:

     cc -o hello_cray hello.cpp
     ./hello_cray

   Note: normally you should always run codes via the batch job system.

3. Change the compiler into AMD Clang

     module load PrgEnv-amd
     module list 

   Proceed then on compiling and running the code:

     cc -o hello_amd hello.cpp
     ./hello_amd

   Note that we use still the same `cc` wrapper

4. One can change back to the Cray compiler by loading the `PrgEnv-cray` module:

     module load PrgEnv-cray
     module list

   Here, `module reset` would unload also the LUMI GPU environment (`LUMI/25.09` and `partition/G` modules).

