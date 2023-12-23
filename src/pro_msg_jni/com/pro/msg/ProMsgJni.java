/*
 * Copyright (C) 2018-2019 Eric Tung <libpronet@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of LibProMsg (https://github.com/libpronet/libpromsg)
 */

package com.pro.msg;

/*
 * please refer to "libpronet/pub/inc/pronet/rtp_msg.h"
 *
 * the file's version is [2.3.0]
 */
public class ProMsgJni
{
    public static class PRO_MSG_USER
    {
        public short classId; /* signature: S,  8bits, 1 ~ 255 */
        public long  userId;  /* signature: J, 40bits, 0 ~ ... */
        public int   instId;  /* signature: I, 16bits, 0 ~ 65535 */

        public PRO_MSG_USER() /* signature: ()V */
        {
            classId = 0;
            userId  = 0;
            instId  = 0;
        }

        public PRO_MSG_USER(  /* signature: (SJI)V */
            short cid,
            long  uid,
            int   iid
            )
        {
            classId = cid;
            userId  = uid;
            instId  = iid;
        }
    }

    public interface MsgClientListener
    {
        /*
         * signature: (JLcom/pro/msg/ProMsgJni$PRO_MSG_USER;Ljava/lang/String;)V
         */
        void msgClientOnOk(
            long         msgClient,
            PRO_MSG_USER myUser,
            String       myPublicIp
            );

        /*
         * signature: (J[BILcom/pro/msg/ProMsgJni$PRO_MSG_USER;)V
         */
        void msgClientOnRecv(
            long         msgClient,
            byte[]       buf,
            int          charset,
            PRO_MSG_USER srcUser
            );

        /*
         * signature: (JIIZ)V
         */
        void msgClientOnClose(
            long    msgClient,
            int     errorCode,
            int     sslCode,
            boolean tcpConnected
            );

        /*
         * signature: (JJ)V
         */
        void msgClientOnHeartbeat(
            long msgClient,
            long peerAliveTick
            );
    }

    public interface MsgServerListener
    {
        /*
         * signature: (JLcom/pro/msg/ProMsgJni$PRO_MSG_USER;Ljava/lang/String;)V
         */
        void msgServerOnOkUser(
            long         msgServer,
            PRO_MSG_USER user,
            String       userPublicIp
            );

        /*
         * signature: (JLcom/pro/msg/ProMsgJni$PRO_MSG_USER;II)V
         */
        void msgServerOnCloseUser(
            long         msgServer,
            PRO_MSG_USER user,
            int          errorCode,
            int          sslCode
            );

        /*
         * signature: (JLcom/pro/msg/ProMsgJni$PRO_MSG_USER;J)V
         */
        void msgServerOnHeartbeatUser(
            long         msgServer,
            PRO_MSG_USER user,
            long         peerAliveTick
            );

        /*
         * signature: (J[BILcom/pro/msg/ProMsgJni$PRO_MSG_USER;)V
         */
        void msgServerOnRecvMsg(
            long         msgServer,
            byte[]       buf,
            int          charset,
            PRO_MSG_USER srcUser
            );
    }

    public static native void getCoreVersion(
        short[] major_1,
        short[] minor_1,
        short[] patch_1
        );

    public static native boolean init(int threadCount); /* 1 ~ (2/20) ~ 100 */

    public static native void fini();

    /*---------------------------------------------------------------------*/

    public static native long msgClientCreate(
        MsgClientListener listener,
        String            configFileName,
        short             mmType,     /* = 0, 11 ~ 20 */
        String            serverIp,   /* = null */
        int               serverPort, /* = 0, 1 ~ 65535 */
        PRO_MSG_USER      user,       /* = null */
        String            password,   /* = null */
        String            localIp     /* = null */
        );

    public static native void msgClientDelete(long client);

    public static native short msgClientGetMmType(long client);

    public static native PRO_MSG_USER msgClientGetUser(long client);

    public static native String msgClientGetSslSuite(long client);

    public static native String msgClientGetLocalIp(long client);

    public static native int msgClientGetLocalPort(long client);

    public static native String msgClientGetRemoteIp(long client);

    public static native int msgClientGetRemotePort(long client);

    public static native boolean msgClientSendMsg(
        long           client,
        byte[]         buf,
        int            charset, /* 0 ~ 65535 */
        PRO_MSG_USER[] dstUsers /* count <= 255 */
        );

    public static native boolean msgClientSendMsg2(
        long           client,
        byte[]         buf1,
        byte[]         buf2,    /* = null */
        int            charset, /* 0 ~ 65535 */
        PRO_MSG_USER[] dstUsers /* count <= 255 */
        );

    public static native void msgClientSetOutputRedline(
        long client,
        long redlineBytes
        );

    public static native long msgClientGetOutputRedline(long client);

    public static native long msgClientGetSendingBytes(long client);

    public static native boolean msgClientReconnect(long client);

    /*---------------------------------------------------------------------*/

    public static native long msgServerCreate(
        MsgServerListener listener,
        String            configFileName,
        short             mmType,        /* = 0, 11 ~ 20 */
        int               serviceHubPort /* = 0, 1 ~ 65535 */
        );

    public static native void msgServerDelete(long server);

    public static native short msgServerGetMmType(long server);

    public static native int msgServerGetServicePort(long server);

    public static native String msgServerGetSslSuite(
        long         server,
        PRO_MSG_USER user
        );

    public static native long msgServerGetUserCount(long server);

    public static native void msgServerKickoutUser(
        long         server,
        PRO_MSG_USER user
        );

    public static native boolean msgServerSendMsg(
        long           server,
        byte[]         buf,
        int            charset, /* 0 ~ 65535 */
        PRO_MSG_USER[] dstUsers /* count <= 255 */
        );

    public static native boolean msgServerSendMsg2(
        long           server,
        byte[]         buf1,
        byte[]         buf2,    /* = null */
        int            charset, /* 0 ~ 65535 */
        PRO_MSG_USER[] dstUsers /* count <= 255 */
        );

    public static native void msgServerSetOutputRedline(
        long server,
        long redlineBytes
        );

    public static native long msgServerGetOutputRedline(long server);

    public static native long msgServerGetSendingBytes(
        long         server,
        PRO_MSG_USER user
        );

    static
    {
        System.loadLibrary("pro_shared");
        System.loadLibrary("pro_net");
        System.loadLibrary("pro_rtp");
        System.loadLibrary("pro_msg_jni");
    }
}
