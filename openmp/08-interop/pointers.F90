program pointers
  use, intrinsic :: iso_c_binding
  implicit none

  integer, parameter :: n = 1024
  real(8), allocatable :: x(:)

  allocate(x(n))

  print '(A, Z16)', "address of x in host: ", transfer(c_loc(x(1)), int(0, kind=c_intptr_t))

  !$omp target data map(to: x(1:n))
  !$omp target data use_device_addr(x)
  print '(A, Z16)', "address of x in dev:  ", transfer(c_loc(x(1)), int(0, kind=c_intptr_t))
  !$omp end target data
  !$omp end target data

  deallocate(x)
end program pointers
