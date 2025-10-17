program pointers
  use, intrinsic :: iso_c_binding
  implicit none

  integer, parameter :: n = 1024
  real(8), allocatable :: x(:)
  type(c_ptr) :: x_host, x_dev

  allocate(x(n))

  x_host = c_loc(x(1))
  print '(A, Z0)', "address of x in host: ", transfer(x_host, int(0, kind=c_intptr_t))

  !$omp target data map(to: x(1:n))
  !$omp target data use_device_ptr(x)
  x_dev = c_loc(x(1))
  print '(A, Z0)', "address of x in dev:  ", transfer(x_dev, int(0, kind=c_intptr_t))
  !$omp end target data
  !$omp end target data

  deallocate(x)
end program pointers
