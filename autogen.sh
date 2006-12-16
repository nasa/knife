#!/bin/sh
#Run to generate bootstrp AutoTools and run configure in a archetecture dir

arch=`uname`

echo "Setting up $arch..."

if [ ! -d $arch ]; then
  echo
  echo "Creating \"$arch\" directory..."
  mkdir $arch
  echo
fi

echo "Running bootstrap ..."
./bootstrap

top_level=`pwd`

echo "Running ../configure --prefix=$top_level $* ..."
( cd $arch && ../configure --prefix=$top_level $* )

