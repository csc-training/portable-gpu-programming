## Discussion

### Exercise: Offload to GPU

1. A trace file is provided for reference: `sync.json`.

   With the default synchronous execution, we see that the GPU kernel launches at the host
   go hand in hand with the GPU kernel executions.

   During `write_array()`, nothing happens on GPU.

2. A trace file of the updated code is provided for reference: `async.json`.

   We see now that during `write_array()`, GPU is busy computing forward.
   The host launches kernels to the queue and then waits at the implicit barriers at
   memory copies.

   Note that `depend` clauses are needed for the correctness.
   Without them, the kernels from different iterations could execute in parallel
   on different streams.
