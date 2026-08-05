! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program pointers
  implicit none

  integer, parameter :: n = 1024
  real(8), allocatable :: x(:)
  integer(8) :: addr

  allocate(x(n))

  addr = loc(x)
  print '(A, Z16, A, I16)', "printing from host the address of x in host: ", addr, " = ", addr

  !$omp target data map(to: x(1:n))

    !$omp target data use_device_addr(x)
    addr = loc(x)
    print '(A, Z16, A, I16)', "printing from host the address of x in host: ", addr, " = ", addr
    !$omp end target data

    ! The next print is split to two lines
    ! as printing from GPU in Fortran is limited on LUMI
    !$omp target
    addr = loc(x)
    print *, "printing from dev  the address of x in dev:"
    print *, addr
    !$omp end target

  !$omp end target data

  deallocate(x)
end program pointers
