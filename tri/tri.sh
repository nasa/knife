#!/usr/bin/env bash

f90=/usr/local/pkgs/g95_0.92_64.S/bin/g95

${f90} -o tri tri.f90 || exit 1

env G95_ENDIAN=BIG ./tri

mv cylinder-unf.tri cylinder-unf-big.tri

od -D -f cylinder-unf-big.tri

env G95_ENDIAN=LITTLE ./tri

mv cylinder-unf.tri cylinder-unf-little.tri

od -D -f cylinder-unf-little.tri

head -1 cylinder-ascii.tri > cylinder-header.tri

od -D -f cylinder-header.tri
