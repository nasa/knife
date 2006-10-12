# -*- Makefile -*-

warning_flags=-pedantic-errors -Wall -Wextra -Werror

lib_LIBRARIES = libknife.a

libknife_a_CFLAGS=$(warning_flags)
libknife_a_SOURCES = \
	knife_definitions.h \
	adj.h adj.c \
	array.h array.c \
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
	subtri.h subtri.c

libknife_a_includedir = @prefix@/include
libknife_a_include_HEADERS = \
	knife_definitions.h \
	adj.h \
	array.h \
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
	subtri.h

bin_PROGRAMS = knife

knife_CFLAGS=$(warning_flags)
knife_SOURCES = knife_main.c
knife_LDADD = libknife.a -lm

EXTRA_DIST = build_ruby_extension.rb RubyExtensionBuilder.rb test_runner.rb \
	adj_ruby.c adj_test.rb \
	primal_ruby.c primal_test.rb \
	node_ruby.c node_test.rb 

TESTS = test_runner.rb

