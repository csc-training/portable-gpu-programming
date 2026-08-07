<!--
SPDX-FileCopyrightText: 2019 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

## Message exchange

In this exercise we practice message (data) passing between MPI processes.
The provided [exchange.cpp](exchange.cpp) code sends `msgsize` integers from 
one MPI task to another (from rank 0 to 1) and vice versa. Investigate the code
and try to understand how it is working.

1. Compile the code (with `CC` or `mpicxx`) and run it with two MPI tasks. Investigate
   the output.

2. Modify the code so that `msgsize` is the same as `arraysize`, compile, and try to run 
   it again. You should notice that program deadlocks, so after a short while 
   kill the Slurm job with `scancel <jobid>`.

   Reason for the deadlock is the both MPI tasks try to send but none of them is receiving
   (with small messages most MPI implementations use some internal buffering, which allows
    `MPI_Send` to complete even though receive has not posted yet.).

   Modify the code so that in one of the MPI tasks the order of `MPI_Send` and `MPI_Recv` is switched,
    *i.e.* one task starts by sending and another one with receiving. Compile the code and run it again,
   you should notice that the program no longer deadlocks.

