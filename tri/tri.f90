
program main

  implicit none

! single precision (IEEE 754)
  integer, parameter :: system_r4=selected_real_kind(6, 37)
! double precision (IEEE 754)
  integer, parameter :: system_r8=selected_real_kind(15, 307)

  integer, parameter :: dp = system_r8 ! default precision kind

  integer :: nnode, nface
  integer :: i, j

  character(80) :: filename

  real(dp), dimension(:,:), allocatable :: xyz
  integer, dimension(:,:), allocatable :: v

  integer :: f

  continue

  filename = 'cylinder-ascii.tri'

  f = 37

  open(f, file=filename, form='formatted', status='old' )

  read(f,*) nnode, nface
  write(*,*)" nnode ",nnode
  write(*,*)" nface ",nface
  allocate( xyz(3,nnode) )
  allocate( v(4,nface) )

  read(f,*) ( (xyz(i,j), i=1,3) ,j=1,nnode) 
  read(f,*) ( (v(i,j), i=1,3) ,j=1,nface) 
  read(f,*) (  v(4,j),j=1,nface) 

  close(f)

  filename = 'cylinder-unf.tri'

  open(f, file=filename, form='unformatted', status='unknown' )

  write(f) nnode, nface
  write(f) ( (xyz(i,j), i=1,3) ,j=1,nnode) 
  write(f) ( (v(i,j), i=1,3) ,j=1,nface) 
  write(f) (  v(4,j),j=1,nface) 

  close(f)

end program main
