! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

#include "helper_functions.F90"

program axpy
  use helper_functions
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

  ! Initialization
  alpha = 3.0d0
  do i = 1, n
    frac = 1.0d0 / real(n - 1, kind=8)
    x(i) = real(i - 1, kind=8) * frac
    y(i) = real(i - 1, kind=8) * frac * 100.0d0
  end do

  ! Print input values
  print '(A)', "Input:"
  print '(A, F8.4)', "a = ", alpha
  call print_array("x", x)
  call print_array("y", y)

  ! Calculate axpy
  !$omp target teams distribute parallel do
  do i = 1, n
    y(i) = y(i) + alpha * x(i)
  end do
  !$omp end target teams distribute parallel do

  ! Print output values
  print '(A)', "Output:"
  call print_array("y", y)

  deallocate(x, y)

end program axpy

