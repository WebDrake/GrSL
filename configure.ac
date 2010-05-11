AC_INIT([GrSL],[development version])
AC_CONFIG_SRCDIR([sampling/gsl_sampling.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror gnu])

AC_CONFIG_HEADERS([config.h])

GRSL_LT_VERSION="0:0:0"
AC_SUBST(GRSL_LT_VERSION)

dnl Check for programs
AC_LANG(C)
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_LIBTOOL

dnl Check for compiler features
AC_C_SIZE_T
AC_C_INLINE

dnl Disable unnecessary libtool tests for FORTRAN and Java
define([AC_LIBTOOL_LANG_F77_CONFIG],[:])dnl
define([AC_LIBTOOL_LANG_GCJ_CONFIG],[:])dnl

PKG_CHECK_MODULES(DEPS,[gsl >= 1.8])
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

GRSL_CFLAGS="-I$includedir $DEPS_CFLAGS"
GRSL_LDFLAGS="$DEPS_LIBS"
GRSL_LIBS="-L$libdir -lgrsl $GRSL_LDFLAGS"

AC_SUBST([GRSL_CFLAGS])
AC_SUBST([GRSL_LDFLAGS])
AC_SUBST([GRSL_LIBS])

AM_CFLAGS = -I$(top_builddir)
AM_LDFLAGS = $(GRSL_LDFLAGS)

AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_LDFLAGS])

CFLAGS="$CFLAGS -ansi -pedantic -Wall -Werror"

AC_CONFIG_FILES([grsl.pc gsl/Makefile sampling/Makefile Makefile])
AC_OUTPUT
