! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

subroutine run(n, niter, normmax)
  use omp_lib
  use helper_functions
  implicit none
  !$omp requires unified_shared_memory
  integer, intent(in) :: n, niter
  real(8), intent(in) :: normmax
  integer(kind=8) :: nx, ny
  integer :: i, j, it, write_flag
  real(8), allocatable :: f(:,:), u(:,:), unew(:,:), tmp(:,:)
  real(8) :: h2, t0, t1, norm2max, norm2, diff
  character(len=20) :: filename

  print '(A, I0, A, I0)', &
    "Using N = ", n, ", niter = ", niter
  nx = n
  ny = n
  h2 = 1.0
  norm2max = normmax * normmax

  allocate(f(ny, nx), u(ny, nx), unew(ny, nx))

  ! Initialize arrays
  call create_input(f)
  u = 0.0
  unew = 0.0

  ! Write initial arrays
  write(filename, '(A,I6.6,A)') 'u', 0, '.bin'
  call write_array(filename, u)
  call write_array('f.bin', f)

  ! Iterate
  t0 = omp_get_wtime()

!$omp parallel num_threads(2)
!$omp single

  do it = 1, niter

    ! Stencil update
    !$omp target
    !$omp teams distribute parallel do collapse(2)
    do j = 2, nx - 1
      do i = 2, ny - 1
        unew(i,j) = 0.25 * (u(i+1,j) + u(i-1,j) + u(i,j+1) + u(i,j-1) - h2 * f(i,j))
      end do
    end do
    !$omp end teams distribute parallel do
    !$omp end target

    ! Swap the arrays
    call move_alloc(u, tmp)
    call move_alloc(unew, u)
    call move_alloc(tmp, unew)

    ! Check converge
    if (mod(it, 100) == 0) then
      norm2 = 0.0d0
      !$omp target map(tofrom: norm2) private(diff) ! map needed here for correctness
      !$omp teams distribute parallel do collapse(2) reduction(+:norm2)
      do j = 2, nx - 1
        do i = 2, ny - 1
          diff = u(i,j) - unew(i,j)
          norm2 = norm2 + diff * diff
        end do
      end do
      !$omp end teams distribute parallel do
      !$omp end target

      print '(I6.6, A, F0.6)', &
        it, ": ", norm2

      if (norm2 < norm2max) then
        print '(A)', "Converged"
        exit
      end if
    end if

    ! Write data
    if (mod(it, 1000) == 0) then
      ! Write in a separate host thread
      ! TODO: Not working with USM. We would need to copy data to
      !       a separate buffer and use that for writing
      !       if we wanted to overlap I/O and computation.
      write(filename, '(A,I6.6,A)') 'u', it, '.bin'
      call write_array(filename, u)
    end if

  end do

  !$omp taskwait

!$omp end single
!$omp end parallel

  t1 = omp_get_wtime()

  ! Write final result
  i = ny / 2
  j = nx / 2
  print '(A, I0, A, I0, A, F0.6)', &
    "u[", i, ",", j, "] = ", u(i,j)
  print '(A, F0.3, A)', &
    "Time spent: ", t1 - t0, " s"
  call write_array("u_end.bin", u)

  deallocate(f, u, unew)
end subroutine run


program main
  implicit none
  integer :: n, niter, nrep, i, iostat
  real(8) :: normmax
  character(len=32) :: arg

  ! Default values
  n = 1024
  niter = 500
  nrep = 3
  normmax = 10.0

  ! Command-line argument parsing
  call get_command_argument(1, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) n
    if (iostat /= 0 .or. n < 1) then
      print *, 'Size needs to be greater than zero.'
      stop 1
    end if
  end if
  call get_command_argument(2, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) niter
    if (iostat /= 0 .or. niter < 1) then
      print *, "Number of iterations needs to be greater than zero."
      stop 1
    end if
  end if
  call get_command_argument(3, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) nrep
    if (iostat /= 0 .or. nrep < 1) then
      print *, "Number of repetitions needs to be greater than zero."
      stop 1
    end if
  end if
  call get_command_argument(4, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) normmax
    if (iostat /= 0 .or. normmax <= 0) then
      print *, "Max norm needs to be greater than zero."
      stop 1
    end if
  end if

  do i = 0, nrep - 1
    print '(A, I0)', &
      "RUN ", i
    call run(n, niter, normmax)
  end do
end program main
