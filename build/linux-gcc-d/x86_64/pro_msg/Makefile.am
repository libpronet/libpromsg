prolibdir = ${prefix}/libpromsg/lib
proincdir = ${prefix}/libpromsg/include/promsg

#############################################################################

prolib_LIBRARIES = libpro_msg.a

proinc_HEADERS = ../../../../src/pro_msg/msg_client.h  \
                 ../../../../src/pro_msg/msg_client2.h \
                 ../../../../src/pro_msg/msg_server.h

libpro_msg_a_SOURCES = ../../../../src/pro_msg/msg_client.cpp      \
                       ../../../../src/pro_msg/msg_client2.cpp     \
                       ../../../../src/pro_msg/msg_reconnector.cpp \
                       ../../../../src/pro_msg/msg_server.cpp

libpro_msg_a_CPPFLAGS = -I${prefix}/libpronet/include

libpro_msg_a_CFLAGS   = -fPIC
libpro_msg_a_CXXFLAGS = -fPIC
