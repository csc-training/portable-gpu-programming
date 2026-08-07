<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

## Tasks

1. See `heat-1.{c,F90}`. A single `target update` statement is enough to transfer
   the data to CPU for writing the data.

   From the profile we see that GPU is idling during `write_array()`.

2. See `heat-2.{c,F90}`.

   Note that this code relies on separate memory allocations for `u` on
   the host and device to overlap the computation and I/O. Thus, it doesn't work correctly
   when using host (CPU threads) as the offload target or using GPU with unified memory.
   For these use cases, a separate memory copy should be done.

   From the profile, we see that now during `write_array()`, GPU is busy computing forward.
   The host launches kernels to the queue and then waits at the implicit barriers at
   memory copies.

