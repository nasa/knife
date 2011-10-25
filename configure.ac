# -*- Autoconf -*-

AC_PREREQ(2.53)

define([git_describe], esyscmd([sh -c "git describe --tag | tr -d '\n'"]))dnl

AC_INIT([knife], [git_describe], [fun3d-support@lists.nasa.gov])
AC_CONFIG_SRCDIR([src/knife_definitions.h])
AM_INIT_AUTOMAKE

AC_PROG_RANLIB

AC_PROG_CC
AM_PROG_CC_C_O
AC_HEADER_STDC

AC_OUTPUT( \
	   Makefile \
	   src/Makefile \
	   util/Makefile \
	   tri/Makefile \
	   )
