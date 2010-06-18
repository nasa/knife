#!/usr/bin/env bash

f90=/usr/local/pkgs/g95_0.92_64.S/bin/g95

${f90} -o tri tri.f90 || exit 1

./tri
