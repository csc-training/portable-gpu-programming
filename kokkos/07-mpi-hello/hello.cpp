#include <Kokkos_Core.hpp>
#include <mpi.h>
#include <iostream>

int main(int argc, char* argv[]) {

  MPI_Init(&argc, &argv);
  Kokkos::initialize(argc, argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
 
  std::ostringstream configuration;  
  Kokkos::DefaultExecutionSpace().print_configuration(configuration);

// Try to print in synchronized manner
  for (int i=0; i < size; i++) {
    if (rank == i)
      std::cout << "MPI rank " << rank << " : " << configuration.str() << std::flush;
    
    MPI_Barrier(MPI_COMM_WORLD);
  }
  
  Kokkos::finalize();

  return 0;
}
