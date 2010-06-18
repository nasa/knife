
program main

  integer :: nnode, nface

  character(80) :: filename

  integer :: f

  continue

  filename = 'cylinder-ascii.tri'

  f = 37

  open(f, file=filename, form='formatted', status='old' )

  read(f,*) nnode, nface
  write(*,*)" nnode ",nnode
  write(*,*)" nface ",nface

  close(f)

end program main
