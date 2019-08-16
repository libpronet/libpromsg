#!/bin/sh

rm -rf ./pro_msg/.deps
rm -rf ./pro_msg_jni/.deps
rm -rf ./cfg/.deps

if [ "${JAVA_HOME}" = "" ]; then

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
./configure                             \
CPPFLAGS="-DNDEBUG                      \
          -D_GNU_SOURCE                 \
          -D_LIBC_REENTRANT             \
          -D_REENTRANT"                 \
CFLAGS="  -O2 -Wall -march=nocona -m64" \
CXXFLAGS="-O2 -Wall -march=nocona -m64" \
LDFLAGS="" $@

rm -f ./configure
