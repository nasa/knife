# -*- Makefile -*-

warning_flags=-pedantic-errors -Wall -Wextra -Werror

lib_LIBRARIES = libknife.a

libknife_a_CFLAGS=$(warning_flags)
libknife_a_SOURCES = \
	knife_definitions.h \
	adj.h adj.c \
	primal.h primal.c
libknife_a_includedir = @prefix@/include
libknife_a_include_HEADERS = \
	knife_definitions.h \
	adj.h \
	primal.h

bin_PROGRAMS = knife

knife_CFLAGS=$(warning_flags)
knife_SOURCES = knife_main.c
knife_LDADD = libknife.a

EXTRA_DIST = build_ruby_extension.rb RubyExtensionBuilder.rb test_runner.rb \
	adj_ruby.c adj_test.rb \
	primal_ruby.c primal_test.rb
