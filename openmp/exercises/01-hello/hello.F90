! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

program hello
  use omp_lib
  implicit none

  print '(A)', "Hello from host!"

  !$omp target
  ! The next print is split to multiple lines
  ! as printing from GPU in Fortran is limited.
  ! The output will be rather messy when there are multiple teams and threads

  ! print '(A,A,I0,A,I0,A,I0,A,I0)', &
  !       "Hello from device! I'm", &
  !       " team ", omp_get_team_num(), "/", omp_get_num_teams(), &
  !       " thread ", omp_get_thread_num(), "/", omp_get_num_threads()
  print *, "Hello from device! I'm"
  print *, "team"
  print *, omp_get_team_num()
  print *, "/"
  print *, omp_get_num_teams()
  print *, "thread"
  print *, omp_get_thread_num()
  print *, "/"
  print *, omp_get_num_threads()

  !$omp end target

end program hello
