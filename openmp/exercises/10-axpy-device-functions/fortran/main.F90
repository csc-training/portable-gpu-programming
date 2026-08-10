! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

#include "helper_functions.F90"

program main
  use helper_functions
  use kernels
  implicit none
  character(len=32) :: arg
  integer :: i, n
  real(8) :: alpha, frac, t0, t1
  real(8), allocatable :: x(:), y(:)

  ! Array size
  n = 102400
  call get_command_argument(1, arg)
  if (len_trim(arg) > 0) then
    read(arg, *) n
  end if
  print '(A, I0)', "Array size n = ", n

  allocate(x(n), y(n))

  !$omp target data map(alloc: x(1:n)) map(from: y(1:n))
    ! Initialization
    alpha = 3.0d0
    !$omp target teams distribute parallel do
    do i = 1, n
      frac = 1.0d0 / real(n - 1, kind=8)
      x(i) = real(i - 1, kind=8) * frac
      y(i) = real(i - 1, kind=8) * frac * 100.0d0
    end do
    !$omp end target teams distribute parallel do

    ! Print input values
    print '(A)', "Input:"
    print '(A, F8.4)', "a = ", alpha
    !$omp target update from(x(1:n)) from(y(1:n))
    call print_array("x", x)
    call print_array("y", y)

    ! Calculate axpy
    !$omp target teams distribute parallel do
    do i = 1, n
      y(i) = axpy(alpha, x(i), y(i))
    end do
    !$omp end target teams distribute parallel do
  !$omp end target data

  ! Print output values
  print '(A)', "Output:"
  call print_array("y", y)

  deallocate(x, y)

end program main
