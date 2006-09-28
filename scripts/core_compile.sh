#!/usr/bin/env bash

fun_loc=$HOME/FUN3D/`uname -n`

( cd $fun_loc/LibF90 && make -j 4 ) && \
( cd $fun_loc/FUN3D_90/ && /usr/local/pkgs/mpich_intel.8.1/bin/mpif90 -I../LibF90 -I../PHYSICS_DUMMY -I../PHYSICS_DEPS    -o nodet_mpi  adjust_alpha_module.o agglomerate_module.o check_jacobian_module.o main.o -L../PHYSICS_DEPS -lFUN3DPhysicsDeps -L../PHYSICS_DUMMY -lFUN3DPhysics -L../LibF90 -lFUN3DCore )

