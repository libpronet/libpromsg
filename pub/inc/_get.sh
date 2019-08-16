#!/bin/sh

THIS_DIR=$(dirname $(readlink -f "$0"))

cp ${THIS_DIR}/../../src/pro_msg/msg_client.h                   ${THIS_DIR}/promsg/
cp ${THIS_DIR}/../../src/pro_msg/msg_server.h                   ${THIS_DIR}/promsg/

cp ${THIS_DIR}/../../src/pro_msg_jni/com/pro/msg/ProMsgJni.java ${THIS_DIR}/com/pro/msg/
