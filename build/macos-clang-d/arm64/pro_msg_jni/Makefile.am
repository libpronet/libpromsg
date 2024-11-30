prolibdir = ${prefix}/libpromsg/lib
proincdir = ${prefix}/libpromsg/include/com/pro/msg

#############################################################################

prolib_PROGRAMS = libpro_msg_jni.so

proinc_HEADERS = ../../../../src/pro_msg_jni/com/pro/msg/ProMsgJni.java

libpro_msg_jni_so_SOURCES = ../../../../src/pro_msg_jni/com_pro_msg_ProMsgJni.cpp \
                            ../../../../src/pro_msg_jni/jni_util.cpp              \
                            ../../../../src/pro_msg_jni/msg_client_jni.cpp        \
                            ../../../../src/pro_msg_jni/msg_server_jni.cpp

libpro_msg_jni_so_CPPFLAGS = -I${prefix}/libpronet/include \
                             -I"${JAVA_HOME}/include"      \
                             -I"${JAVA_HOME}/include/linux"

libpro_msg_jni_so_CFLAGS   = -fPIC
libpro_msg_jni_so_CXXFLAGS = -fPIC

libpro_msg_jni_so_LDFLAGS = -shared -Wl,-rpath,.:${prefix}/libpronet/lib
libpro_msg_jni_so_LDADD   =

LIBS = ../pro_msg/libpro_msg.a   \
       -L${prefix}/libpronet/lib \
       -lpro_rtp                 \
       -lpro_net                 \
       -lpro_util                \
       -lpro_shared              \
       -lmbedtls                 \
       -lpthread                 \
       -lc
