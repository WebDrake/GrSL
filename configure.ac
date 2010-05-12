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
AC_C_INLINE

if test "$ac_cv_c_inline" != no ; then
dnl Check for "extern inline", using a modified version of the test
dnl for AC_C_INLINE from acspecific.mt
dnl
   AC_CACHE_CHECK([for GNU-style extern inline], ac_cv_c_extern_inline,
   [ac_cv_c_extern_inline=no
   AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[extern $ac_cv_c_inline double foo(double x);
   extern $ac_cv_c_inline double foo(double x) { return x + 1.0 ; } ;
   double foo (double x) { return x + 1.0 ; };]], [[  foo(1.0)  ]])],[ac_cv_c_extern_inline="yes"],[])
   ])

   if test "$ac_cv_c_extern_inline" != no ; then
      AC_DEFINE(HAVE_INLINE,[1],[Define if you have inline])
   else
      AC_CACHE_CHECK([for C99-style inline], ac_cv_c_c99inline,
      [ac_cv_c_c99inline=no
      dnl next line is a necessary condition
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[extern inline void* foo() { foo(); return &foo ; };]],
      [[  return foo() != 0 ]])],[ac_cv_c_c99inline="yes"],[])
      dnl but not sufficient, extern must work but inline on its own should not
      if test "$ac_cv_c_c99inline" != no ; then
            AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[inline void* foo() { foo(); return &foo ; };]],
            [[  return foo() != 0 ]])],[],ac_cv_c_c99inline="no")
      fi
      ])
      if test "$ac_cv_c_c99inline" != no ; then
         AC_DEFINE(HAVE_INLINE,[1],[Define if you have inline])
         AC_DEFINE(HAVE_C99_INLINE,[1],[Define if you have inline with C99 behavior])
      fi
   fi
fi

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

CFLAGS="$CFLAGS -std=c99 -pedantic -Wall -Werror"

AH_BOTTOM([#define RETURN_IF_NULL(x) if (!x) { return ; }
])

AC_CONFIG_FILES([grsl.pc gsl/Makefile sampling/Makefile Makefile])
AC_OUTPUT
