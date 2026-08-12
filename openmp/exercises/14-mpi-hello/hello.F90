! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program hello
  use mpi_f08
  use omp_lib
  implicit none
  integer :: rc, rank, noderank, ntasks, namelen
  type(mpi_comm) :: commnode
  character(len=MPI_MAX_PROCESSOR_NAME) :: procname
  integer devicecount, my_device

  call mpi_init(rc)
  call mpi_comm_size(MPI_COMM_WORLD, ntasks, rc)
  call mpi_comm_rank(MPI_COMM_WORLD, rank, rc)

  ! find name of the processor (node) that this rank is running on.
  call mpi_get_processor_name(procname, namelen, rc)

  ! Determine the node local rank
  call mpi_comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, commnode, rc);  
  call mpi_comm_rank(commnode, noderank, rc)

  devicecount = omp_get_num_devices()
  call omp_set_default_device(mod(nodeRank, deviceCount))
  my_device = omp_get_default_device()

  write(*,'(A,I3,A,A,A,I3,A,I3)') &
    'Rank ', rank, ' on node ', procname(:namelen), ' :', &
    devicecount, ' GPUs in total, using GPU ', my_device

  ! write(*,*) 'Rank ', rank, ' on node ', procname(:namelen), ' :', devicecount, ' GPUs in total, using GPU ', my_device

end program hello
