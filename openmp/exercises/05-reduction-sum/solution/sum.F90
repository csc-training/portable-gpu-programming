! SPDX-FileCopyrightText: 2010 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program sum
  use omp_lib
  implicit none
  character(len=32) :: arg
  integer :: i, n
  real(8) :: t0, t1, total

  ! Array size
  n = 100000
  call get_command_argument(1, arg)
  if (len_trim(arg) > 0) then
    read(arg, *) n
  end if
  print '(A, I0)', "Array size: ", n

  ! Start timing
  t0 = omp_get_wtime()

  ! Calculate sum
  total = 0.0d0
  !$omp target map(tofrom: total)
  !$omp teams distribute parallel do reduction(+:total)
  do i = 1, n
    total = total + sin(real(i - 1, kind=8))
  end do
  !$omp end teams distribute parallel do
  !$omp end target

  ! End timing
  t1 = omp_get_wtime()

  print '(A, F0.6)', "Sum: ", total
  print '(A, F0.3, A)', "Calculation took ", (t1 - t0) * 1.0d3, " milliseconds"

end program sum
