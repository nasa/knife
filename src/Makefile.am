# -*- Makefile -*-

lib_LIBRARIES    = libknife.a

warning_flags = -pedantic -Wall -Wextra -Werror

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

libknife_a_CFLAGS  = $(warning_flags)
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

knife_convert_CFLAGS  = $(warning_flags)
knife_convert_SOURCES = knife_convert.c
knife_convert_LDADD   = libknife.a -lm

knife_vis_CFLAGS  = $(warning_flags)
knife_vis_SOURCES = knife_vis.c
knife_vis_LDADD   = libknife.a -lm

EXTRA_DIST = build_ruby_extension.rb RubyExtensionBuilder.rb test_runner.rb \
	adj_ruby.c adj_test.rb \
	primal_ruby.c primal_test.rb \
	node_ruby.c node_test.rb 

TESTS = test_runner.rb

