prolibdir = ${prefix}/libpromsg/lib
proincdir = ${prefix}/libpromsg/include/com/pro/msg
procppdir = ${prefix}/libpromsg/include/cpp

#############################################################################

prolib_PROGRAMS = libpro_msg_jni.so

proinc_HEADERS = ../../../../src/pro_msg_jni/com/pro/msg/ProMsgJni.java

procpp_DATA = ../../../../src/pro_msg_jni/msg_client.cpp \
              ../../../../src/pro_msg_jni/msg_client.h   \
              ../../../../src/pro_msg_jni/msg_server.cpp \
              ../../../../src/pro_msg_jni/msg_server.h

libpro_msg_jni_so_SOURCES = ../../../../src/pro_msg_jni/com_pro_msg_ProMsgJni.cpp \
                            ../../../../src/pro_msg_jni/jni_util.cpp              \
                            ../../../../src/pro_msg_jni/msg_client.cpp            \
                            ../../../../src/pro_msg_jni/msg_client_jni.cpp        \
                            ../../../../src/pro_msg_jni/msg_server.cpp            \
                            ../../../../src/pro_msg_jni/msg_server_jni.cpp

libpro_msg_jni_so_CPPFLAGS = -I/usr/local/libpronet/include \
                             -I${JAVA_HOME}/include         \
                             -I${JAVA_HOME}/include/linux

libpro_msg_jni_so_CFLAGS   = -fno-strict-aliasing -fPIC -fvisibility=hidden
libpro_msg_jni_so_CXXFLAGS = -fno-strict-aliasing -fPIC -fvisibility=hidden

libpro_msg_jni_so_LDFLAGS = -shared -Wl,-soname,libpro_msg_jni.so \
                            -Wl,-rpath,.:/usr/local/libpronet/lib \
                            -Wl,--no-undefined
libpro_msg_jni_so_LDADD   =

LIBS = -L/usr/local/libpronet/lib \
       -lpro_rtp                  \
       -lpro_net                  \
       -lpro_util                 \
       -lpro_shared               \
       -lmbedtls                  \
       -lstdc++                   \
       -lrt                       \
       -lpthread                  \
       -lm                        \
       -lgcc                      \
       -lc
