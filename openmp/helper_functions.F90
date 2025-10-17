module helper_functions
  implicit none
  contains

  subroutine print_array(name, x)
    character(len=*), intent(in) :: name
    real(8), intent(in) :: x(:)
    integer :: i, n
    integer, parameter :: PRINT_N = 4

    n = size(x)

    write(*,'(A)', advance='no') trim(name) // " ="
    do i = 1, n
      if (i <= PRINT_N .or. i > n - PRINT_N) then
        write(*,'(" ", F8.4)', advance='no') x(i)
      else if (i == PRINT_N + 1) then
        write(*,'(A)', advance='no') " ..."
      end if
    end do
    write(*,*)
  end subroutine print_array

  subroutine create_input(f)
    implicit none
    real(8), intent(out) :: f(:, :)
    integer :: i, j, ind, nx, ny
    real(8) :: cx, cy, sigma2, kx, ky, dx, dy, r2

    nx = size(f, 2)
    ny = size(f, 1)

    cx = real(nx, kind=8) / 2.0d0
    cy = real(ny, kind=8) / 2.0d0
    sigma2 = 0.05d0 * nx * ny
    kx = 20.0d0 / nx
    ky = 10.0d0 / ny

    do j = 1, nx
      do i = 1, ny
        dx = j - cx
        dy = i - cy
        r2 = dx * dx + dy * dy
        f(i,j) = cos(kx * dx + ky * dy) * exp(-r2 / sigma2)
      end do
    end do
  end subroutine create_input

  subroutine write_array(filename, array, ierr)
    implicit none
    character(len=*), intent(in) :: filename
    real(8), intent(in) :: array(:, :)
    integer, intent(out), optional :: ierr
    integer :: unit, ios, local_err

    open(newunit=unit, file=filename, form='unformatted', access='stream', status='replace', action='write', iostat=ios)
    if (ios /= 0) then
      print *, "Failed to open file"
      local_err = 1
      if (present(ierr)) ierr = local_err
      return
    end if

    write(unit) size(array, kind=8)
    write(unit) array
    close(unit)

    if (ios /= 0) then
      print *, "Failed to write all elements to file"
      local_err = 2
    else
      local_err = 0
    end if

    if (present(ierr)) ierr = local_err
  end subroutine write_array

end module helper_functions
