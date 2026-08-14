! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

#include "heat_helper_functions.F90"

subroutine run(n, niter)
  use mpi_f08
  use omp_lib
  use heat_helper_functions
  use iso_fortran_env, only: output_unit
  implicit none
  integer, intent(in) :: n, niter
  integer :: nx_full, ny_full, nx, ny, nx_inner
  integer :: i, j, it, r
  integer :: ntasks, rank, ierr
  integer :: nghbrs(2)
  real(8), pointer :: u(:,:), unew(:,:), tmp(:,:)
  real(8), allocatable :: u_full(:,:)
  integer :: comm_count, comm_ix_start
  integer, allocatable :: comm_counts(:), comm_displs(:)
  real(8) :: Lx, Ly, alpha, dx, dy, dx2, dy2, dt, rx, ry, t0, t1

  ! Grid size
  nx_full = n
  ny_full = n

  call MPI_Comm_size(MPI_COMM_WORLD, ntasks, ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)

  nghbrs(1) = rank - 1
  nghbrs(2) = rank + 1
  if (rank == 0) nghbrs(1) = MPI_PROC_NULL
  if (rank == ntasks - 1) nghbrs(2) = MPI_PROC_NULL

  nx_inner = calculate_inner_size(nx_full, rank, ntasks)
  nx = nx_inner + 2  ! Add halo and/or boundary conditions to the array
  ny = ny_full

  ! Box size
  Lx = 8.0d0
  Ly = 8.0d0

  ! Diffusivity
  alpha = 0.5d0

  ! Grid spacing
  dx = Lx / (nx_full - 1)
  dy = Ly / (ny_full - 1)
  dx2 = dx * dx
  dy2 = dy * dy

  ! Largest stable time step
  dt = dx2 * dy2 / (2.0d0 * alpha * (dx2 + dy2))

  if (rank == 0) then
    ! Print inputs
    print '(A, I0, A, I0)', &
      "Inputs: n = ", n, ", niter = ", niter
    print '(A, F0.2)', &
      "Diffusivity: ", alpha
    print '(A, F0.2, A, F0.2, A, ES0.2, A, ES0.2)', &
      "Box: ", Lx, " x ", Ly, " discretized with grid spacing ", dx, " x ", dy
    print '(A, ES0.2, A, ES0.2)', &
      "Time propagation until ", dt*niter, " with time step ", dt
  end if

  rx = alpha * dt / dx2
  ry = alpha * dt / dy2

  allocate(u(ny, nx))
  u = 0.0d0

  ! Prepare communication pointers and sizes
  if (rank == 0) then
    ! Communicate also first column (global boundary) in first rank
    comm_ix_start = 1
  else
    ! Skip first column (halo) in other ranks
    comm_ix_start = 2
  end if
  comm_count = calculate_comm_count(nx_full, ny_full, rank, ntasks)

  ! Debug printing for communication
  if (rank == 0) then
    write(*,'(A)') "Debug printing from each rank:"
    call flush(output_unit)
  end if
  call MPI_Barrier(MPI_COMM_WORLD, ierr)
  write(*,'(A, I4, A, I12, A, I12, A, I12, A, I12)') &
    "rank: ", rank, ": displ = ", (comm_ix_start - 1) * ny, &
    ", count = ", comm_count, ", nx = ", nx, ", ny = ", ny
  call flush(output_unit)
  call MPI_Barrier(MPI_COMM_WORLD, ierr)

  if (rank == 0) then
    ! Initialize arrays
    allocate(u_full(ny_full, nx_full))
    call create_input(u_full, Lx, Ly)

    ! Write initial arrays
    call write_array("u_initial.bin", u_full, Lx, Ly)

    ! Calculate sizes to communicate to each rank
    allocate(comm_counts(ntasks), comm_displs(ntasks))
    comm_displs(1) = 0
    do r = 1, ntasks
      comm_counts(r) = calculate_comm_count(nx_full, ny_full, r-1, ntasks)
      if (r > 1) comm_displs(r) = comm_displs(r-1) + comm_counts(r-1)
    end do

    ! Debug printing for communication
    write(*,'(A)') "Debug printing from root:"
    do r = 1, ntasks
      write(*,'(A, I4, A, I12, A, I12)') &
        "root: ", r-1, ": displ = ", comm_displs(r), ", count = ", comm_counts(r)
    end do

    ! Scatter initial array
    call MPI_Scatterv(u_full(1,1), comm_counts, comm_displs, MPI_DOUBLE_PRECISION, &
                      u(1, comm_ix_start), comm_count, MPI_DOUBLE_PRECISION, &
                      0, MPI_COMM_WORLD, ierr)
    deallocate(u_full)
  else
    allocate(comm_counts(0), comm_displs(0))
    ! Scatter initial array
    call MPI_Scatterv(u(1,1), comm_counts, comm_displs, MPI_DOUBLE_PRECISION, &
                      u(1, comm_ix_start), comm_count, MPI_DOUBLE_PRECISION, &
                      0, MPI_COMM_WORLD, ierr)
  end if
  call flush(output_unit)
  call MPI_Barrier(MPI_COMM_WORLD, ierr)

  allocate(unew(ny, nx))
  unew = 0.0d0

  ! Propagate in time
  t0 = omp_get_wtime()

  !$omp target data map(tofrom: u(1:ny,1:nx)) map(to: unew(1:ny,1:nx))
  do it = 1, niter

    ! Halo exchange
    ! Note: this is done before any compute so that the initial values
    ! are correctly filled in the halos too
    !$omp target data use_device_addr(u)
    call MPI_Sendrecv(u(1, 2), ny, MPI_DOUBLE_PRECISION, nghbrs(1), 123, &
                      u(1, nx), ny, MPI_DOUBLE_PRECISION, nghbrs(2), 123, &
                      MPI_COMM_WORLD, MPI_STATUS_IGNORE, ierr)
    call MPI_Sendrecv(u(1, nx-1), ny, MPI_DOUBLE_PRECISION, nghbrs(2), 123, &
                      u(1, 1), ny, MPI_DOUBLE_PRECISION, nghbrs(1), 123, &
                      MPI_COMM_WORLD, MPI_STATUS_IGNORE, ierr)
    !$omp end target data

    ! Stencil update
    !$omp target
    !$omp teams distribute parallel do collapse(2)
    do j = 2, nx - 1
      do i = 2, ny - 1
        unew(i,j) = u(i,j) + rx * (u(i,j+1) - 2*u(i,j) + u(i,j-1)) &
                           + ry * (u(i+1,j) - 2*u(i,j) + u(i-1,j))
      end do
    end do
    !$omp end teams distribute parallel do
    !$omp end target

    ! Swap the arrays
    tmp => u
    u => unew
    unew => tmp

  end do
  !$omp end target data

  t1 = omp_get_wtime()

  ! Write final result
  if (rank == 0) then
    allocate(u_full(ny_full, nx_full))

    ! Gather the array
    call MPI_Gatherv(u(1, comm_ix_start), comm_count, MPI_DOUBLE_PRECISION, &
                     u_full(1,1), comm_counts, comm_displs, MPI_DOUBLE_PRECISION, &
                     0, MPI_COMM_WORLD, ierr)

    i = ny_full / 2
    j = nx_full / 2
    print '(A, I0, A, I0, A, F0.6)', &
      "u[", i, ",", j, "] = ", u_full(i,j)
    print '(A, F0.3, A)', &
      "Time spent: ", t1 - t0, " s"
    call write_array("u_final.bin", u_full, Lx, Ly)

    deallocate(u_full)
  else
    ! Gather the array
    call MPI_Gatherv(u(1, comm_ix_start), comm_count, MPI_DOUBLE_PRECISION, &
                     u(1,1), comm_counts, comm_displs, MPI_DOUBLE_PRECISION, &
                     0, MPI_COMM_WORLD, ierr)
  end if

  deallocate(u, unew, comm_counts, comm_displs)

contains

  integer function calculate_inner_size(n_full, rank, ntasks)
    integer, intent(in) :: n_full, rank, ntasks
    integer :: n_full_inner
    n_full_inner = n_full - 2  ! Remove global boundary condition
    calculate_inner_size = n_full_inner / ntasks + &
                           merge(1, 0, rank < mod(n_full_inner, ntasks))
  end function calculate_inner_size

  integer function calculate_comm_count(nx_full, ny_full, rank, ntasks)
    integer, intent(in) :: nx_full, ny_full, rank, ntasks
    integer :: nx_inner
    nx_inner = calculate_inner_size(nx_full, rank, ntasks)
    calculate_comm_count = ny_full * nx_inner
    if (rank == 0) then
      ! Communicate also global boundary in first
      calculate_comm_count = calculate_comm_count + ny_full
    end if
    if (rank == ntasks - 1) then
      ! Communicate also global boundary in last rank
      ! Note! Different if so that it works correctly with ntasks=1
      calculate_comm_count = calculate_comm_count + ny_full
    end if
  end function calculate_comm_count

end subroutine run


program main
  use mpi_f08
  use omp_lib
  use iso_fortran_env, only: output_unit
  implicit none
  integer :: n, niter, nrep, i, iostat
  integer :: ierr, rank
  integer :: count, device
  character(len=32) :: arg
  character(len=MPI_MAX_PROCESSOR_NAME) :: node_name
  integer :: node_name_len

  call MPI_Init(ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, rank, ierr)
  call MPI_Get_processor_name(node_name, node_name_len, ierr)

  ! Set device per rank
  count = omp_get_num_devices()
  call omp_set_default_device(mod(rank, count))
  device = omp_get_default_device()

  write(*,'(A, I0, A, I0, A, A)') "MPI rank ", rank, " has GPU ", device, &
    " on node ", node_name(:node_name_len)
  call flush(output_unit)
  call MPI_Barrier(MPI_COMM_WORLD, ierr)

  ! Default values
  n = 1024
  niter = 500
  nrep = 3

  ! Command-line argument parsing
  call get_command_argument(1, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) n
    if (iostat /= 0 .or. n < 1) then
      print *, 'Size needs to be greater than zero.'
      call MPI_Finalize(ierr)
      stop 1
    end if
  end if
  call get_command_argument(2, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) niter
    if (iostat /= 0 .or. niter < 0) then
      print *, "Number of iterations need to be non-negative."
      call MPI_Finalize(ierr)
      stop 1
    end if
  end if
  call get_command_argument(3, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) nrep
    if (iostat /= 0 .or. nrep < 1) then
      print *, "Number of repetitions need to be greater than zero."
      call MPI_Finalize(ierr)
      stop 1
    end if
  end if

  do i = 0, nrep - 1
    if (rank == 0) print '(A, I0)', "RUN ", i
    call run(n, niter)
    if (rank == 0) call flush(output_unit)
  end do

  call MPI_Finalize(ierr)
end program main
