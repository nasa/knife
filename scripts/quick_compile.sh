#!/usr/bin/env bash

fun_loc=$HOME/FUN3D/`uname -n`

( cd $fun_loc/LibF90 && make -j 4 ) && \
( cd $fun_loc/PHYSICS_DEPS/ && make -j 4 ) && \
( cd $fun_loc/FUN3D_90/ && make -j 4 )

