#!/bin/sh

THIS_DIR=$(dirname $(readlink -f "$0"))

cp ${THIS_DIR}/../../build/linux-gcc-r/x86/pro_msg_jni/libpro_msg_jni.so ${THIS_DIR}/linux-gcc/x86/
