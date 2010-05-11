noinst_LTLIBRARIES = libgslsampling.la

ACLOCAL_AMFLAGS = -I ../m4

AM_CFLAGS = -I$(top_builddir)
AM_LDFLAGS = $(GRSL_LDFLAGS)

libgslsampling_la_SOURCES = vitter_a.c
libgslsampling_la_includedir = $(includedir)/gsl
libgslsampling_la_include_HEADERS = gsl_sampling.h
