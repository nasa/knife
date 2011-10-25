! f90 -o domain01 domain01.f90 && ./domain01

module kinds
  integer, parameter :: dp = selected_real_kind(P=15)
end module kinds

module fast

contains

  subroutine write_fast(nnodes,ntface,ntet,xyz,inode,itag,c2nt,nbcs,itestr)

    use kinds, only : dp

    implicit none

    integer :: nnodes,ntface,ntet,nbcs

    real(dp), dimension(nnodes,3) :: xyz
    integer,  dimension(ntface,3) :: inode
    integer,  dimension(ntface)   :: itag(ntface)
    integer,  dimension(ntet,4)   :: c2nt
    integer,  dimension(nbcs,2)   :: itestr

    integer :: i, j

    open (unit=60,file='domain01.fgrid')
    open (unit=61,file='domain01.fastbc')

    write(60,'(3i15)')    nnodes,ntface,ntet
    write(60,'(3e25.15)') (xyz(i,1),i=1,nnodes)
    write(60,'(3e25.15)') (xyz(i,2),i=1,nnodes)
    write(60,'(3e25.15)') (xyz(i,3),i=1,nnodes)
    write(60,'(3i15)')    ((inode(i,j),j=1,3),i=1,ntface)
    write(60,'(i15)')     (itag(i),i=1,ntface)
    write(60,'(4i15)')    ((c2nt(i,j),j=1,4),i=1,ntet)

    write(61,*) nbcs                        ! No. of BC's
    do i = 1,nbcs
      write(61,*) itestr(i,1),itestr(i,2)
    enddo

    close(60)
    close(61)

  end subroutine write_fast

end module fast

program main

  use kinds, only : dp
  use fast, only : write_fast

  implicit none

  integer, parameter :: m=6
  integer, parameter :: l=6
  integer, parameter :: n=6
  integer, parameter :: nnodes = l*m*n

  integer, parameter :: ntet = 6*(l-1)*(m-1)*(n-1)
  integer, parameter :: ntface=4*(l-1)*(m-1)+4*(m-1)*(n-1)+4*(n-1)*(l-1)
  integer, parameter :: nbcs=6


  real(dp), dimension(nnodes,3) :: xyz
  integer,  dimension(ntface,3) :: inode
  integer,  dimension(ntface)   :: itag(ntface)
  integer,  dimension(ntet,4)   :: c2nt
  integer,  dimension(nbcs,2)   :: itestr

  real(dp) :: x0, x1
  real(dp) :: y0, y1
  real(dp) :: z0, z1

  integer :: i, j, k, in, it
  integer :: n1, n2, n3, n4, n5, n6, n7, n8
  integer :: is, iseg

  continue

  write(*,*) "enter  boundaries of cube"
  write(*,*) "x0"
  read(*,*) x0
  write(*,*) "x1"
  read(*,*) x1
  write(*,*) "y0"
  read(*,*) y0
  write(*,*) "y1"
  read(*,*) y1
  write(*,*) "z0"
  read(*,*) z0
  write(*,*) "z1"
  read(*,*) z1

  write(*,*) "grid dimensions"
  write(*,'(3i6)') l,m,n
  write(*,*) "nodes"
  write(*,*) nnodes
  write(*,*) "tets"
  write(*,*) ntet

  open (unit=60,file='faux_input')
  write(60,'(i1)') 6
  write(60,'(i1," xplane ",f)') 1, x0
  write(60,'(i1," xplane ",f)') 2, x1
  write(60,'(i1," yplane ",f)') 3, y0
  write(60,'(i1," yplane ",f)') 4, y1
  write(60,'(i1," zplane ",f)') 5, z0
  write(60,'(i1," zplane ",f)') 6, z1
  write(60,'(a)') ''
  close (60)

  do k=1,n
    do j=1,m
      do i=1,l
        in =  i + (j-1)*l + (k-1)*l*m
        xyz(in,1) = x0 + (x1-x0)/real(l-1,dp)*real(i-1,dp)
        xyz(in,2) = y0 + (y1-y0)/real(m-1,dp)*real(j-1,dp)
        xyz(in,3) = z0 + (z1-z0)/real(n-1,dp)*real(k-1,dp)
      enddo
    enddo
  enddo

! calc fast.fc file info

! inv  3k
! visc 4k
! free 5k
! extrap 5005
! back press 5010

! x=x0 i=1
  itestr(1,1) = 1
  itestr(1,2) = 5000

! x=x1 i=l
  itestr(2,1) = 2
  itestr(2,2) = 5000

! y=y0 j=1
  itestr(3,1) = 3
  itestr(3,2) = 5000

! y=y1 j=n
  itestr(4,1) = 4
  itestr(4,2) = 5000

! z=z0 k=1
  itestr(5,1) = 5
  itestr(5,2) = 5000

! z=z1 k=m
  itestr(6,1) = 6
  itestr(6,2) = 5000

! calculate tet nodes

  it=0

  do i=1,l-1
    do j=1,m-1
      do k=1,n-1

        n1 =  (i+0) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n2 =  (i+0) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n3 =  (i+0) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n5 =  (i+1) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n6 =  (i+1) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n7 =  (i+1) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n8 =  (i+1) + ((j+1)-1)*l + ((k+1)-1)*l*m

        it=it+1
        c2nt(it,1)=n2
        c2nt(it,2)=n4
        c2nt(it,3)=n3
        c2nt(it,4)=n7

        it=it+1
        c2nt(it,1)=n1
        c2nt(it,2)=n2
        c2nt(it,3)=n3
        c2nt(it,4)=n7

        it=it+1
        c2nt(it,1)=n1
        c2nt(it,2)=n5
        c2nt(it,3)=n2
        c2nt(it,4)=n7

        it=it+1
        c2nt(it,1)=n2
        c2nt(it,2)=n5
        c2nt(it,3)=n6
        c2nt(it,4)=n7

        it=it+1
        c2nt(it,1)=n2
        c2nt(it,2)=n8
        c2nt(it,3)=n4
        c2nt(it,4)=n7

        it=it+1
        c2nt(it,1)=n2
        c2nt(it,2)=n6
        c2nt(it,3)=n8
        c2nt(it,4)=n7

      enddo
    enddo
  enddo

! calculate bc faces

  is = 0

  iseg = 1

  do i=1, 1
    do j=1,m-1
      do k=1,n-1

        n1 =  (i+0) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n2 =  (i+0) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n3 =  (i+0) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n5 =  (i+1) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n6 =  (i+1) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n7 =  (i+1) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n8 =  (i+1) + ((j+1)-1)*l + ((k+1)-1)*l*m

        is = is + 1
        inode(is,1) = n2
        inode(is,2) = n4
        inode(is,3) = n3
        itag(is) = iseg

        is = is + 1
        inode(is,1) = n1
        inode(is,2) = n2
        inode(is,3) = n3
        itag(is) = iseg

      enddo
    enddo
  enddo

  iseg = 2

  do i= l-1, l-1
    do j=1,m-1
      do k=1,n-1

        n1 =  (i+0) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n2 =  (i+0) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n3 =  (i+0) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n5 =  (i+1) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n6 =  (i+1) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n7 =  (i+1) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n8 =  (i+1) + ((j+1)-1)*l + ((k+1)-1)*l*m

        is = is + 1
        inode(is,1) = n6
        inode(is,2) = n7
        inode(is,3) = n8
        itag(is) = iseg

        is = is + 1
        inode(is,1) = n5
        inode(is,2) = n7
        inode(is,3) = n6
        itag(is) = iseg

      enddo
    enddo
  enddo

  iseg = 3

  do i=1,l-1
    do j=1, 1
      do k=1,n-1

        n1 =  (i+0) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n2 =  (i+0) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n3 =  (i+0) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n5 =  (i+1) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n6 =  (i+1) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n7 =  (i+1) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n8 =  (i+1) + ((j+1)-1)*l + ((k+1)-1)*l*m

        is = is + 1
        inode(is,1) = n1
        inode(is,2) = n7
        inode(is,3) = n5
        itag(is) = iseg

        is = is + 1
        inode(is,1) = n1
        inode(is,2) = n3
        inode(is,3) = n7
        itag(is) = iseg

      enddo
    enddo
  enddo

  iseg = 4

  do i=1,l-1
    do j=m-1, m-1
      do k=1,n-1

        n1 =  (i+0) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n2 =  (i+0) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n3 =  (i+0) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n5 =  (i+1) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n6 =  (i+1) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n7 =  (i+1) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n8 =  (i+1) + ((j+1)-1)*l + ((k+1)-1)*l*m

        is = is + 1
        inode(is,1) = n2
        inode(is,2) = n6
        inode(is,3) = n8
        itag(is) = iseg

        is = is + 1
        inode(is,1) = n2
        inode(is,2) = n8
        inode(is,3) = n4
        itag(is) = iseg

      enddo
    enddo
  enddo


  iseg = 5

  do i=1,l-1
    do j=1,m-1
      do k=1, 1

        n1 =  (i+0) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n2 =  (i+0) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n3 =  (i+0) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n5 =  (i+1) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n6 =  (i+1) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n7 =  (i+1) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n8 =  (i+1) + ((j+1)-1)*l + ((k+1)-1)*l*m

        is = is + 1
        inode(is,1) = n1
        inode(is,2) = n5
        inode(is,3) = n2
        itag(is) = iseg

        is = is + 1
        inode(is,1) = n2
        inode(is,2) = n5
        inode(is,3) = n6
        itag(is) = iseg

      enddo
    enddo
  enddo

  iseg = 6

  do i=1,l-1
    do j=1, m-1
      do k=n-1,n-1

        n1 =  (i+0) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n2 =  (i+0) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n3 =  (i+0) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n4 =  (i+0) + ((j+1)-1)*l + ((k+1)-1)*l*m
        n5 =  (i+1) + ((j+0)-1)*l + ((k+0)-1)*l*m
        n6 =  (i+1) + ((j+1)-1)*l + ((k+0)-1)*l*m
        n7 =  (i+1) + ((j+0)-1)*l + ((k+1)-1)*l*m
        n8 =  (i+1) + ((j+1)-1)*l + ((k+1)-1)*l*m

        is = is + 1
        inode(is,1) = n3
        inode(is,2) = n4
        inode(is,3) = n7
        itag(is) = iseg

        is = is + 1
        inode(is,1) = n4
        inode(is,2) = n8
        inode(is,3) = n7
        itag(is) = iseg

      enddo
    enddo
  enddo


  call write_fast(nnodes,ntface,ntet,xyz,inode,itag,c2nt,nbcs,itestr)

end program main

