# -*- Makefile -*-

lib_LIBRARIES    = libknife.a

library_sources = \
	knife_definitions.h \
	adj.h adj.c \
	array.h array.c \
	set.h set.c \
	primal.h primal.c \
	node.h node.c \
	segment.h segment.c \
	triangle.h triangle.c \
	mask.h mask.c \
	poly.h poly.c \
	surface.h surface.c \
	domain.h domain.c \
	intersection.h intersection.c \
	cut.h cut.c \
	near.h near.c \
	subnode.h subnode.c \
	subtri.h subtri.c \
	loop.h loop.c \
	logger.h logger.c \
	knife_fortran.c

libknife_a_SOURCES = $(library_sources)

libknife_a_includedir = @prefix@/include
libknife_a_include_HEADERS = \
	knife_definitions.h \
	adj.h \
	array.h \
	set.h \
	primal.h \
	node.h \
	segment.h \
	triangle.h \
	mask.h \
	poly.h \
	surface.h \
	domain.h \
	intersection.h \
	cut.h \
	near.h \
	subnode.h \
	subtri.h \
	loop.h \
	logger.h

bin_PROGRAMS = knife-convert knife-vis

EXTRA_PROGRAMS = knife-cut

knife_convert_SOURCES = knife_convert.c
knife_convert_LDADD   = libknife.a -lm

knife_vis_SOURCES = knife_vis.c
knife_vis_LDADD   = libknife.a -lm

knife_cut_SOURCES = knife_cut.c
knife_cut_LDADD   = libknife.a -lm

