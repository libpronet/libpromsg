#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([libpromsg], [0.0.0])
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_CONDITIONAL([BUILD_JNI_SO], [test "x${JAVA_HOME}" != "x"])
AC_CONFIG_FILES([Makefile
                 pro_msg/Makefile
                 pro_msg_jni/Makefile
                 cfg/Makefile])
AC_OUTPUT
