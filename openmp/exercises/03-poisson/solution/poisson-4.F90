! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

subroutine run(n, niter)
  use omp_lib
  use helper_functions
  implicit none
  integer, intent(in) :: n, niter
  integer(kind=8) :: nx, ny
  integer :: i, j, it
  real(8), allocatable :: f(:,:), u(:,:), unew(:,:), tmp(:,:)
  real(8) :: h2, t0, t1
  character(len=20) :: filename

  print '(A, I0, A, I0)', &
    "Using N = ", n, ", niter = ", niter
  nx = n
  ny = n
  h2 = 1.0

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

  !$omp target data map(to: f(1:ny,1:nx)) map(tofrom: u(1:ny,1:nx)) map(to: unew(1:ny,1:nx))
  do it = 1, niter

    ! Stencil update
    !$omp target teams distribute parallel do collapse(2)
    do j = 2, nx - 1
      do i = 2, ny - 1
        unew(i,j) = 0.25 * (u(i+1,j) + u(i-1,j) + u(i,j+1) + u(i,j-1) - h2 * f(i,j))
      end do
    end do
    !$omp end target teams distribute parallel do

    ! Swap the arrays
    call move_alloc(u, tmp)
    call move_alloc(unew, u)
    call move_alloc(tmp, unew)

  end do
  !$omp end target data

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
  use iso_fortran_env, only: output_unit
  implicit none
  integer :: n, niter, nrep, i, iostat
  character(len=32) :: arg

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

  do i = 0, nrep - 1
    print '(A, I0)', &
      "RUN ", i
    call run(n, niter)
    call flush(output_unit)
  end do
end program main
