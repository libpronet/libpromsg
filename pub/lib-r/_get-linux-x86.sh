#!/bin/sh
THIS_MOD=$(readlink -f "$0")
THIS_DIR=$(dirname "${THIS_MOD}")

cp "${THIS_DIR}/../../build/linux-gcc-r/x86/pro_msg/libpro_msg.a"          "${THIS_DIR}/linux-gcc/x86/"
cp "${THIS_DIR}/../../build/linux-gcc-r/x86/pro_msg_jni/libpro_msg_jni.so" "${THIS_DIR}/linux-gcc/x86/"
