#!/bin/sh

rm -rf ./pro_msg/.deps
rm -rf ./pro_msg_jni/.deps
rm -rf ./cfg/.deps

if [ -z "${JAVA_HOME}" ]; then

  echo " ERROR! JAVA_HOME is not set. "

else

  echo " JAVA_HOME: ${JAVA_HOME} "

fi

#
# configure.ac ---> aclocal.m4
#
aclocal --force

#
# configure.ac + aclocal.m4 ---> configure
#
autoconf --force

#
# configure.ac + Makefile.am ---> Makefile.in
#
automake --add-missing --force-missing --foreign

#
# Makefile.in ---> Makefile
#
./configure \
CPPFLAGS="-D_DEBUG          \
          -D_GNU_SOURCE     \
          -D_LIBC_REENTRANT \
          -D_REENTRANT"     \
CFLAGS="  -g -O0 -Wall -fno-strict-aliasing -fvisibility=hidden -march=nocona -m64" \
CXXFLAGS="-g -O0 -Wall -fno-strict-aliasing -fvisibility=hidden -march=nocona -m64" \
LDFLAGS="" $@

rm -f ./configure
