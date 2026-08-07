<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Exercise: Heat equation: Overlapping I/O with GPU execution

In this exercise we practise more advanced use of asynchronous execution.

Continuing from the previous heat equation code, we have added
an I/O step that writes the array on every 1000th iteration to the disk.

The problem is that this doesn't work as all the data is only on GPU.
Let's fix this!

The [solution directory](solution/) contains a model solution and discussion on the exercises below.

## Tasks

1. Add suitable data update clauses to fix the I/O step.

   Run the profiler on the fixed code. The `write_array()` function has timing markers that can be enabled during compilation by adding
   compilation options `-DTRACE` (Roihu) or `-DTRACE -lroctx64` (LUMI).

2. (Bonus) From the timeline we see that the GPU is idling while the file is being written.

   Fix this issue by overlapping writing the data and GPU kernel execution.

   Hint: you can utilize multiple host (CPU) threads: one is scheduling the work for GPU device and CPU threads
   and one is doing the I/O.
