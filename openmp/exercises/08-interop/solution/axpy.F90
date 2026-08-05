! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

subroutine run(n)
  use, intrinsic :: iso_c_binding
  use helper_functions
  use hipblas_bindings
  implicit none
  integer, intent(in) :: n
  real(8), allocatable :: x(:), y(:)
  real(8) :: alpha
  integer :: i
  real(8) :: frac
  type(c_ptr) :: handle
  integer(c_int) :: status

  print '(A, I0)', "Using N = ", n

  ! Create handle for hipblas
  call hipblasCreate(handle)
  call hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_HOST)

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
  !$omp target data map(to: x(1:n)) map(tofrom: y(1:n))
  !$omp target data use_device_addr(x, y)
  call hipblasDaxpy(handle, n, alpha, x, 1, y, 1)
  !$omp end target data
  !$omp end target data

  ! Print output values
  print '(A)', "Output:"
  call print_array("y", y)

  deallocate(x, y)

  ! Destroy hipblas handle
  call hipblasDestroy(handle)

end subroutine run


program axpy
  implicit none
  integer :: n, arg_len, iostat
  character(len=32) :: arg

  ! Default value
  n = 102400

  ! Get command line argument if provided
  call get_command_argument(1, arg)
  if (len_trim(arg) > 0) then
    read(arg, *, iostat=iostat) n
    if (iostat /= 0 .or. n < 1) then
      print *, 'Size needs to be greater than zero.'
      stop 1
    end if
  end if

  call run(n)

end program axpy
