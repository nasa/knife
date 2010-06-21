#!/usr/bin/env bash

f90=missing-g95
if [ -x /usr/local/bin/g95 ]
then
  f90=/usr/local/bin/g95
fi
if [ -x /usr/local/pkgs/g95_0.92_64.S/bin/g95 ]
then
  f90=/usr/local/pkgs/g95_0.92_64.S/bin/g95
fi

src=../Linux/src

( cd ${src} && make )

${f90} -o tri tri.f90 || exit 1

env G95_ENDIAN=BIG ./tri

mv cylinder-unf.tri cylinder-unf-big.tri

od -D -f cylinder-unf-big.tri

env G95_ENDIAN=LITTLE ./tri

mv cylinder-unf.tri cylinder-unf-little.tri

od -D -f cylinder-unf-little.tri

head -1 cylinder-ascii.tri > cylinder-header.tri

od -D -f cylinder-header.tri

${src}/knife-convert -i cylinder-unf-big.tri
${src}/knife-convert -i cylinder-unf-little.tri
${src}/knife-convert -i cylinder-header.tri

