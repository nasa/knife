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

( cd ${src} && make ) || exit 1

${f90} -o tri tri.f90 || exit 1

env G95_ENDIAN=BIG ./tri

mv cylinder-unf.tri cylinder-unf-big.tri

od -D -f cylinder-unf-big.tri | head -10

env G95_ENDIAN=LITTLE ./tri

mv cylinder-unf.tri cylinder-unf-little.tri

od -D -f cylinder-unf-little.tri  | head -10

${src}/knife-convert -i cylinder-unf-big.tri
${src}/knife-convert -i cylinder-unf-little.tri
${src}/knife-convert -i cylinder-ascii.tri

rm -f primal.*
${src}/knife-convert cylinder-unf-little.tri
diff cylinder-ascii.tri primal.tri || exit 1

rm -f primal.*
${src}/knife-convert cylinder-unf-big.tri
diff cylinder-ascii.tri primal.tri || exit 1

rm -f cylinder-unf-big.tri cylinder-unf-little.tri
rm -f massoud.t primal.*
rm -f tri

