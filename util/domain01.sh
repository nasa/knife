#!/usr/bin/env bash

( mpif90 -o brick brick.f90 && ./brick ) || exit

rm -rf fast.mod kinds.mod

rm -rf brick

