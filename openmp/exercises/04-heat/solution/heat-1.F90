! SPDX-FileCopyrightText: 2025 CSC - IT Center for Science Ltd. <www.csc.fi>
!
! SPDX-License-Identifier: MIT

#include "helper_functions.F90"

subroutine run(n, niter)
  use omp_lib
  use helper_functions
  implicit none
  integer, intent(in) :: n, niter
  integer(kind=8) :: nx, ny
  integer :: i, j, it
  real(8), pointer :: u(:,:), unew(:,:), tmp(:,:)
  real(8) :: Lx, Ly, alpha, dx, dy, dx2, dy2, dt, rx, ry, t0, t1

  ! Grid size
  nx = n
  ny = n

  ! Box size
  Lx = 8.0d0
  Ly = 8.0d0

  ! Diffusivity
  alpha = 0.5d0

  ! Grid spacing
  dx = Lx / (nx - 1)
  dy = Ly / (ny - 1)
  dx2 = dx * dx
  dy2 = dy * dy

  ! Largest stable time step
  dt = dx2 * dy2 / (2.0d0 * alpha * (dx2 + dy2))

  ! Print inputs
  print '(A, I0, A, I0)', &
    "Inputs: n = ", n, ", niter = ", niter
  print '(A, F0.2)', &
    "Diffusivity: ", alpha
  print '(A, F0.2, A, F0.2, A, ES0.2, A, ES0.2)', &
    "Box: ", Lx, " x ", Ly, " discretized with grid spacing ", dx, " x ", dy
  print '(A, ES0.2, A, ES0.2)', &
    "Time propagation until ", dt*niter, " with time step ", dt

  rx = alpha * dt / dx2
  ry = alpha * dt / dy2

  allocate(u(ny, nx), unew(ny, nx))

  ! Initialize arrays
  call create_input(u, Lx, Ly)
  unew = 0.0d0

  ! Write initial arrays
  call write_array("u_initial.bin", u, Lx, Ly)

  ! Propagate in time
  t0 = omp_get_wtime()

  ! TODO 2: Transfer the needed arrays to GPU before the main loop

  do it = 1, niter

    ! Stencil update
    !$omp target map(to: u(1:ny,1:nx)) map(tofrom: unew(1:ny,1:nx))
    !$omp teams distribute parallel do collapse(2)
    do j = 2, nx - 1
      do i = 2, ny - 1
        unew(i,j) = u(i,j) + rx * (u(i,j+1) - 2*u(i,j) + u(i,j-1)) &
                           + ry * (u(i+1,j) - 2*u(i,j) + u(i-1,j))
      end do
    end do
    !$omp end teams distribute parallel do
    !$omp end target

    ! Swap the arrays
    tmp => u
    u => unew
    unew => tmp

  end do

  ! TODO 2: Transfer the needed arrays back to CPU after the main loop

  t1 = omp_get_wtime()

  ! Write final result
  i = ny / 2
  j = nx / 2
  print '(A, I0, A, I0, A, F0.6)', &
    "u[", i, ",", j, "] = ", u(i,j)
  print '(A, F0.3, A)', &
    "Time spent: ", t1 - t0, " s"
  call write_array("u_final.bin", u, Lx, Ly)

  deallocate(u, unew)
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
    print '(A, I0)', "RUN ", i
    call run(n, niter)
    call flush(output_unit)
  end do
end program main
