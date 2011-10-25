#!/usr/bin/env bash

l=9 # number of nodes in x direction
m=3 # number of nodes in y direction
n=9 # number of nodes in z direction

echo $l $m $n

# extent of the background grid

x0=-1.0
x1=5.0
y0=-0.2
y1=0.2
z0=-1.0
z1=5.0

# compile and run the fromtran code

( ifort -o domain01 domain01.f90 ) || exit

printf "$l\n$m\n$n\n$x0\n$x1\n$y0\n$y1\n$z0\n$z1\n" | ./domain01

rm -rf fast.mod kinds.mod uniform_grid.mod domain01.o

rm -rf domain01

