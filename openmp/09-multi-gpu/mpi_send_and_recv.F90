program mpi_send_and_recv
  use mpi
  ! use mpi_f08 ! does not work on LUMI!
  use omp_lib
  implicit none

  integer :: rank, size, ierr
  integer :: count, device, i
  integer, parameter :: N = 1024*1024
  real(8), allocatable :: data(:)

  call MPI_Init(ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, size, ierr)

  ! Get number of devices and set device per rank
  count = omp_get_num_devices()
  call omp_set_default_device(mod(rank, count))
  device = omp_get_default_device()

  print *, 'Hello from MPI rank', rank, '/', size, 'with GPU', device, '/', count

  allocate(data(N))

  !$omp target data map(alloc: data(1:N))

  if (rank == 0) then
    ! Compute on device
    !$omp target teams distribute parallel do
    do i = 1, N
        data(i) = 42.0
    end do
    !$omp end target teams distribute parallel do

    ! Send data from device
    !$omp target data use_device_addr(data)
    call MPI_Send(data, N, MPI_DOUBLE_PRECISION, 1, 123, MPI_COMM_WORLD, ierr)
    !$omp end target data

    !$omp target update from(data(1:N))
    print *, 'Rank', rank, 'sent', data(1), '..', data(N)

  else if (rank == 1) then

    ! Receive data to device
    !$omp target data use_device_addr(data)
    call MPI_Recv(data, N, MPI_DOUBLE_PRECISION, 0, 123, MPI_COMM_WORLD, MPI_STATUS_IGNORE, ierr)
    !$omp end target data

    !$omp target update from(data(1:N))
    print *, 'Rank', rank, 'received', data(1), '..', data(N)
  end if
  !$omp end target data

  deallocate(data)

  call MPI_Finalize(ierr)

end program mpi_send_and_recv
