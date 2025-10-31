<!--
SPDX-FileCopyrightText: 2021 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Parallel calculation of π
Starting from the mpi example of computing [`pi`](https://github.com/csc-training/mpi-introduction/edit/main/parallel-pi/solution/pi.cpp) and using this [Intel example](https://www.intel.com/content/www/us/en/developer/articles/technical/compile-and-run-mpi-programs-using-dpcpp-language.html) construct a code which computes the value of `pi` using 2 or more gpus, with 1 GPU device per MPI task.

<!-- Adapted from material by EPCC https://github.com/EPCCed/archer2-MPI-2020-05-14 -->

## Computing of π

An approximation to the value of π can be calculated from the following
expression

<!--
\frac{\pi}{4} = \int_0^1 \frac{dx}{1+x^2} \approx \frac{1}{N} \sum_{i=1}^N \frac{1}{1+\left( \frac{i-\frac{1}{2}}{N}\right)^2}
-->
![img](img/eq1.png)

where the answer becomes more accurate with increasing N. As each term is independent, the summation over *i* can be parallelized nearly trivially. The work is divided in `ntasks` so that rank 0 does i=1, 2, ..., N / ntasks, rank 1 does i=N / ntasks + 1, N / ntasks + 2, ... , *etc.* (we assume that N is evenly divisible by the number of processes). Each tasks computes their own sum. Once finished with the calculation, all ranks (expect rank 0) send their partial sum to rank 0, which then calculates the final result and prints it out.

## Task

Starting from the mpi parallel code [pi.cpp](cpu/pi.cpp), make a version that performs the calculation using sycl for the local reduction similar to the [reduction with buffer](../05-reduction/reduction_simple_buffer.cpp) or [reduction with usm](../05-reduction/reduction_simple_usm.cpp) examples.
Remember to assign 1 GPU to 1 task similar to the [MPI examples](../08-mpi/) taking into account that each Mahti GPU node has 4 GPUs and each LUMI-G node has 8 GPUs.
