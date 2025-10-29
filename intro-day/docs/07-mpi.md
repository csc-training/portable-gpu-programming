---
title: Introduction to MPI
event: Portable GPU Programming 2025
lang:  en
---

# Outline

- The idea of Message Passing Interface (MPI)
- MPI in practice: Creating, compiling, and running an MPI program
- MPI communicator
- Simple point-to-point communication


# Introduction to MPI {.section}

# Message Passing Interface (MPI)

- MPI is an application programming interface (API) for distributed parallel computing
- MPI programs are portable and scalable
  - The same program can run on different types of computers, from laptops to supercomputers or even across different computers
- MPI is flexible and comprehensive
  - Large (hundreds of procedures)
  - Concise (a few procedures is enough to get a functional code)


# MPI standard and implementations

- MPI is a standard, first version (1.0) published in 1994, latest (5.0) in 2025
  - <https://www.mpi-forum.org/docs/>
- The MPI standard is implemented by different MPI implementations
  - [OpenMPI](http://www.open-mpi.org/)
  - [MPICH](https://www.mpich.org/)
  - [Intel MPI](https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/mpi-library.html)


# Processes and threads

![](img/processes-threads-highlight-proc.png){.center width=85%}

<div class="column">

**Process**

- Independent execution units
- Have their own state information and *own memory* address space

</div>

<div class="column">

**Thread**

- A single process may contain multiple threads
- Have their own state information, but *share* the *same memory* address space

</div>


# Execution model in MPI

- MPI program is launched as a set of *independent processes*
- Typically, every process executes the *same program executable (code)*, but they have a *different state*
  - Each process has a unique *rank* (an index ranging from 0 to N-1)
  - Processes can perform different tasks and handle different data based on their rank
- MPI supports also dynamic spawning of processes and launching *different* programs communicating with each other
  - Not covered here


# Data model in MPI

- All variables and data structures are local to the process
- Processes exchange data explicitly by sending and receiving messages

![](img/data-model.png){.center width=100%}


# Interlude

Message passing game


# MPI in practice {.section}

# Programming MPI

- The MPI standard defines interfaces to C and Fortran
  - No C++ bindings in the standard, C++ programs use the C interface
  - There are unofficial bindings to Python, Rust, R, ...
- The standard defines three different Fortran interfaces
  - The modern way: `use mpi_f08` (used here)
  - Older interfaces: `use mpi` or `include 'mpif.h'` (deprecated in MPI-4.1)
  - See a warning later


# Minimal MPI program

<div class="column">
**C**

```c++
// Include MPI header file
#include <mpi.h>

int main(int argc, char *argv[]) {
    // Start by calling MPI_Init()
    MPI_Init(&argc, &argv);

    ...  // program code

    // Call MPI_Finalize() before exiting
    MPI_Finalize();
}


```
</div>

<div class="column">
**Fortran 2008**

```fortranfree
program my_prog
  ! Use MPI module
  use mpi_f08    ! Modern Fortran !
  implicit none

  ! Start by calling mpi_init()
  call mpi_init()

  ... ! program code

  ! Call mpi_finalize() before exiting
  call mpi_finalize()
end program my_prog
```
</div>
- Mandatory: wrap the MPI program inside `MPI_Init()` and `MPI_Finalize()`!


# MPI function/procedure call convention

<div class="column">
**C**

```c++
error_code = MPI_Xxxx(parameter, ...);
```
- Case-sensitive
- Return value is the error code
</div>
<div class="column">
**Fortran 2008**

```fortranfree
call mpi_xxxx(parameter, ..., error_code)
```
- Case-insensitive
- Last argument is the error code (optional with `mpi_f08`)
</div>

# Runtime errors in MPI

MPI has a built-in error checker with (mostly) useful error messages.

- No need to check error codes manually
- Default behavior is to abort on an error: all MPI errors are **fatal**
- Can change error handling method if you really want to...
  - Rarely needed and not covered on these slides
  - Spec **does not** guarantee that any error can be recovered from

<small>See eg. `message-length` exercise</small>

# Warning on old Fortran codes!

- Older Fortran codes might have `use mpi` or `include 'mpif.h'`
- No argument checking (or only partial checking)
- Last error code argument is **mandatory**!
  - Forgetting the error code argument ~~can~~ <u>will</u> cause hard-to-find bugs!
- Use modern `mpi_f08` for any new code


# MPI function syntax on the slides

MPI_Function(`input_arg`{.input}, `output_arg`{.output})
  : Input arguments are in **`red`{.input}** and output arguments in **`blue`{.output}**

<p>
- In C the output arguments are always pointers!
- See MPI implementation references for detailed function definitions:
  - <https://docs.open-mpi.org/en/v5.0.x/man-openmpi/man3/index.html>
  - <https://www.mpich.org/static/docs/v4.2.x/>
  - `man MPI_Function` on LUMI (e.g., `man MPI_Init`)

# First two MPI functions: Initializing and finalizing

MPI_Init(...)
  : Initializes the MPI execution environment
  : Note! C interface include command line arguments

MPI_Finalize()
  : Terminates the MPI execution environment

<p>
- Demo: `hello.c`


# Compiling an MPI program

Use MPI compiler wrappers for easy compiling:

```bash
mpicc  -o my_mpi_prog my_mpi_code.c
mpicxx -o my_mpi_prog my_mpi_code.cpp
mpif90 -o my_mpi_prog my_mpi_code.F90
```

- MPI-related options are automatically included
  - For example with GCC and OpenMPI on Debian, `mpicc` is equivalent to<br>`gcc -I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi -I/usr/lib/x86_64-linux-gnu/openmpi/include -pthread -L/usr/lib/x86_64-linux-gnu/openmpi/lib -lmpi`


# Compiling an MPI program on LUMI

On LUMI, Cray compiler wrappers replace `mpi*` wrappers:

```bash
cc  -o my_mpi_prog my_mpi_code.c
CC  -o my_mpi_prog my_mpi_code.cpp
ftn -o my_mpi_prog my_mpi_code.F90
```

- Use these on LUMI instead of the `mpi*` wrappers!
- Cray wrappers will pick the correct compiler and compilation options based on the loaded modules


# Launching an MPI program

- MPI program needs to be launched with a process launcher (typically `mpiexec` or `mpirun` in a standalone MPI installation)
  - This launches the requested number of processes, initializes their state (e.g., rank), and the MPI communication framework
- On LUMI and Mahti, use the `srun` launcher command instead
  - This integrates with SLURM (understands allocated resources, knows where and how to distribute the processes, ...)
- In general, the launcher command to use changes from supercomputer to supercomputer
  - Check the user documentation of your system

# MPI communicator {.section}

# MPI communicator

- An object connecting a group of processes
- Specifies the communication context
- A special communicator `MPI_COMM_WORLD` contains all the processes
- A process can belong to multiple communicators
  - Custom communicators can be defined


# Information about the MPI communicator

MPI_Comm_size(`comm`{.input}, `size`{.output})
  : Determines the size of the group associated with a communicator

MPI_Comm_rank(`comm`{.input}, `rank`{.output})
  : Determines the rank of the calling process in the communicator

<p>
- Demo: `hello_rank.c`

# Synchronization

MPI_Barrier(`comm`{.input})
  : Waits until all ranks within the communicator reaches the call

<p>
- Demo: `barrier.c`

# Point-to-point communication in MPI {.section}

# Communication in MPI

<div class="column">

- Data is local to the MPI processes
    - They need to *communicate* to coordinate work
- Point-to-point communication
    - Messages are sent between two processes
- Collective communication
    - Involving a number of processes at the same time

</div>

<div class="column">

![](img/communication-schematic.png){.center width=50%}

</div>

# MPI point-to-point operations

- One process *sends* a message to another process that *receives* it with `MPI_Send` and `MPI_Recv` routines
- Sends and receives in a program should match – one receive per send
- Each message contains
  - Data                                                                                       - A *buffer*, the *datatype* of the data elements, and the *number* of the data elements
  - Envelope
    - The ranks of the *source* and *destination* processes
    - An identification number for the message (*tag*)
    - Used *communicator*

# MPI point-to-point operations

MPI_Send(`buffer`{.input}, `count`{.input}, `datatype`{.input}, `dest`{.input}, `tag`{.input}, `comm`{.input})
  : Performs a blocking send
  : **Note!** The `count` parameter is the number of elements to send

MPI_Recv(`buffer`{.output}, `count`{.input}, `datatype`{.input}, `source`{.input}, `tag`{.input}, `comm`{.input}, `status`{.output})
  : Performs a blocking receive
  : **Note!** The `count` parameter is the **maximum** number of elements to receive
  : The `status` parameter is discussed later; use special `MPI_STATUS_IGNORE` for now

<p>
- Demo: `send_and_recv.c`


# Buffers in MPI

- The `buffer` arguments are *memory addresses*
- MPI assumes a contiguous chunk of memory
  - The `count` elements are send starting from the address
  - The received elements are stored starting from the address
- In C/C++ `buffer` is pointer
  - See next slides                                                                        - In Fortran arguments are passed by reference and variables can be passed as such to MPI calls
  - Note: be careful if passing non-contiguous array segmens such as <br>`a(1, 1:N)`

# MPI datatypes

- On a low level, MPI sends and receives stream of bytes
- MPI datatypes specify how the bytes should be interpreted
  - Allows data conversions in heterogenous environments (*e.g.* little endian to big endian)
- MPI has a number of predefined basic datatypes corresponding to C or Fortran datatypes
  - Listed in the next slides
- Datatype `MPI_BYTE` for raw bytes is available both in C and Fortran
  - Portability can be an issue when using `MPI_BYTE` - be careful
- One can also define custom datatypes for communicating complex data


# Common MPI datatypes specific for C/C++

| MPI type     |  C type       |
| ------------ | ------------- |
| `MPI_CHAR`   | `signed char` |
| `MPI_SHORT`  | `short int`   |
| `MPI_INT`    | `int`         |
| `MPI_LONG`   | `long int`    |
| `MPI_FLOAT`  | `float`       |
| `MPI_DOUBLE` | `double`      |

# Case study: parallel sum on two processes

<div class=column>
![](img/case_study_left-01.png){.center width=45%}
<p>
- Demo: `parallel_sum.c`
</div>
<div class=column>
- Array initially on process #0 (P0)
- Parallel algorithm:
    1. **Scatter**:
    P0 sends half of the array to process P1

    2. **Compute**:
    P0 & P1 sum independently their segments

    3. **Reduction**:
    Partial sum on P1 is sent to P0 and
    P0 sums the partial sums

</div>

# Case study: parallel sum on two processes

<div class=column>
![](img/case_study_left-03.png){.center width=45%}
<p>
- Demo: `parallel_sum.c`
</div>
<div class=column>
**Step 1**: Scatter array
<p>
![](img/case_study_right-02.png){.center width=90%}
</div>

# Case study: parallel sum on two processes

<div class=column>
![](img/case_study_left-04.png){.center width=45%}
<p>
- Demo: `parallel_sum.c`
</div>
<div class=column>
**Step 2**: Compute the sum in parallel
<p>
![](img/case_study_right-03.png){.center width=90%}
</div>

# Case study: parallel sum on two processes

<div class=column>
![](img/case_study_left-06.png){.center width=45%}
<p>
- Demo: `parallel_sum.c`
</div>
<div class=column>
**Step 3.1**: Gather partial sums
<p>
![](img/case_study_right-05.png){.center width=90%}
</div>

# Case study: parallel sum on two processes

<div class=column>
![](img/case_study_left-07.png){.center width=45%}
<p>
- Demo: `parallel_sum.c`
</div>
<div class=column>
**Step 3.2**: Compute the total sum
<p>
![](img/case_study_right-06.png){.center width=90%}
</div>


# Blocking routines and deadlocks

- `MPI_Send` and `MPI_Recv` are *blocking* routines
- `MPI_Send` returns once the send buffer can be safely read and written to
  - **Note!** This does not necessarily mean that the communication has taken place when `MPI_Send` returns <br>
    → See exercise
- `MPI_Recv` returns once it has received the message in the receive buffer
- In general, the completion may depend on other processes → risk for *deadlocks*
  - For example, all processes are waiting in `MPI_Recv` but no-one is sending <br>
    → the program is stuck forever (deadlock)

# Summary {.section}

# Summary

- In parallel programming with MPI, the key concept is a set of independent processes
- Data is always local to the process
- Processes can exchange data by sending and receiving messages
- MPI defines procedures for communication and synchronization between processes

# Summary

- Point-to-point communication = messages are sent between two MPI processes
- Point-to-point operations enable any parallel communication pattern
  - `MPI_Send` and `MPI_Recv` would be (in principle) enough
- `MPI_Send` and `MPI_Recv` are blocking routines
  - Beware of deadlocks
