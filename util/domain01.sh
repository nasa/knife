#!/usr/bin/env bash

( mpif90 -o domain01 domain01.f90 && ./domain01 ) || exit

rm -rf fast.mod kinds.mod

rm -rf domain01

