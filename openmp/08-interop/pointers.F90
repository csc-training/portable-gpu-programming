! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program pointers
  implicit none

  integer, parameter :: n = 1024
  real(8), allocatable :: x(:)

  allocate(x(n))

  print '(A, Z16)', "printing from host the address of x in host: ", loc(x)

  !$omp target data map(to: x(1:n))

    !$omp target data use_device_addr(x)
    print '(A, Z16)', "printing from host the address of x in dev:  ", loc(x)
    !$omp end target data

! Not working in Fortran?
!    !$omp target
!    print '(A, Z16)', "printing from dev  the address of x in dev:  ", loc(x)
!    !$omp end target

  !$omp end target data

  deallocate(x)
end program pointers
