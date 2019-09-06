prolibdir = ${prefix}/libpromsg/lib
proincdir = ${prefix}/libpromsg/include/promsg

#############################################################################

prolib_LIBRARIES = libpro_msg.a

proinc_HEADERS = ../../../../src/pro_msg/msg_client.h  \
                 ../../../../src/pro_msg/msg_client2.h \
                 ../../../../src/pro_msg/msg_server.h

libpro_msg_a_SOURCES = ../../../../src/pro_msg/msg_client.cpp  \
                       ../../../../src/pro_msg/msg_client2.cpp \
                       ../../../../src/pro_msg/msg_server.cpp

libpro_msg_a_CPPFLAGS = -I../../../../../libpronet/pub/inc

libpro_msg_a_CFLAGS   = -fno-strict-aliasing -fPIC -fvisibility=hidden
libpro_msg_a_CXXFLAGS = -fno-strict-aliasing -fPIC -fvisibility=hidden
