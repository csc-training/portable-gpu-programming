## Hello World with MPI

1. Compile the provided [hello.cpp](hello.cpp) (or [hello.f90](hello.f90) with the proper MPI wrapper:
   - In LUMI: `CC -o hello hello.cpp` (`ftn -o hello hello.f90`)
   - In Mahti: `mpicxx -o hello hello.cpp` (`mpif90 -o hello hello.f90`)
2. Run the code via Slurm with different number of MPI tasks and investigate the output.
   You may use the provided `job_lumi.sh` / `job_mahti.sh` Slurm batch job scripts
