# -*- Makefile -*-

lib_LIBRARIES = libknife.a

warning_flags=-pedantic-errors -Wall -Wextra -Werror

libknife_a_CFLAGS=$(warning_flags)
libknife_a_SOURCES = \
	knife_defs.h \
	adj.h adj.c

bin_PROGRAMS = knife

knife_CFLAGS=$(warning_flags)
knife_SOURCES = knife_main.c
knife_LDADD = libknife.a
