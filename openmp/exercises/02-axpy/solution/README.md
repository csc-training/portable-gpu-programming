<!--
SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>

SPDX-License-Identifier: CC-BY-4.0
-->

# Discussion

*Note for instructors: use `run*.sh` scripts to generate all the output files.*

## Tasks

1. Code compiles and runs without issues.

2. See `axpy.{c,F90}`.
   See [data](data/) directory for outputs.

3. See `axpy-nomap.{c,F90}` for codes with missing data mapping clauses.
   See [data](data/) directory for outputs.

   The Fortran code still works as the Fortran array is aware of its size and hence
   OpenMP can do the data transfers correctly.
   Note though that the NVIDIA HPC compiler generates unnecessary
   `tofrom` mapping for `x` although only `to` is needed.
   On LUMI, the Cray compiler generates `to` implicitly.

   On Roihu, the C code works without explicit mapping as on the GH200 superchip
   both CPU and GPU memories are visible on the GPU.
   On LUMI, the C code does not work without explicit mapping.

4. See `*.lst` files in [data](data/) directory.


### Bonus tasks: Offload to CPU threads

The code compiles and runs without issues on with CPU target too.
