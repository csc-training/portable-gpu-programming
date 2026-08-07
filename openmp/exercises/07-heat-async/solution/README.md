<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

## Tasks

1. With the default synchronous execution, we see that the GPU kernel launches at the host
   go hand in hand with the GPU kernel executions.

2. See `heat.{c,F90}`.

   Note that `depend` clauses are needed for the correctness.
   Without them, the kernels from different iterations could execute in parallel
   on different streams.
