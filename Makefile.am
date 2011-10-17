SUBDIRS = gsl sampling
SUBLIBS = sampling/libgslsampling.la

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -I$(top_builddir)
AM_LDFLAGS = $(GRSL_LDFLAGS)

lib_LTLIBRARIES = libgrsl.la
libgrsl_la_SOURCES =
libgrsl_la_LIBADD = $(SUBLIBS) $(AM_LDFLAGS)
libgrsl_la_LDFLAGS = -version-info $(GRSL_LT_VERSION)
libgrsl_la_includedir = $(includedir)/gsl

bin_PROGRAMS = grsl-test
grsl_test_SOURCES = grsl-test.c
grsl_test_LDADD = libgrsl.la

