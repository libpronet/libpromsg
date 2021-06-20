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

#if !defined(____MSG_CLIENT2_H____)
#define ____MSG_CLIENT2_H____

#include "msg_client.h"
#include "pronet/pro_memory_pool.h"
#include "pronet/rtp_base.h"
#include "pronet/rtp_msg.h"

/////////////////////////////////////////////////////////////////////////////
////

class CMsgClient2;

/////////////////////////////////////////////////////////////////////////////
////

class IMsgClientObserver
{
public:

    virtual ~IMsgClientObserver() {}

    virtual unsigned long PRO_CALLTYPE AddRef() = 0;

    virtual unsigned long PRO_CALLTYPE Release() = 0;

    virtual void PRO_CALLTYPE OnOkMsg(
        CMsgClient2*        msgClient,
        const RTP_MSG_USER* myUser,
        const char*         myPublicIp
        ) = 0;

    virtual void PRO_CALLTYPE OnRecvMsg(
        CMsgClient2*        msgClient,
        const void*         buf,
        unsigned long       size,
        PRO_UINT16          charset,
        const RTP_MSG_USER* srcUser
        ) = 0;

    virtual void PRO_CALLTYPE OnCloseMsg(
        CMsgClient2* msgClient,
        long         errorCode,
        long         sslCode,
        bool         tcpConnected
        ) = 0;

    virtual void PRO_CALLTYPE OnHeartbeatMsg(
        CMsgClient2* msgClient,
        PRO_INT64    peerAliveTick
        ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

class CMsgClient2 : public CMsgClient
{
public:

    static CMsgClient2* CreateInstance();

    bool Init(
        IMsgClientObserver* observer,
        IProReactor*        reactor,
        const char*         configFileName,
        RTP_MM_TYPE         mmType,     /* = 0 */
        const char*         serverIp,   /* = NULL */
        unsigned short      serverPort, /* = 0 */
        const RTP_MSG_USER* user,       /* = NULL */
        const char*         password,   /* = NULL */
        const char*         localIp     /* = NULL */
        );

    void Fini();

private:

    CMsgClient2();

    virtual ~CMsgClient2();

    virtual void PRO_CALLTYPE OnOkMsg(
        IRtpMsgClient*      msgClient,
        const RTP_MSG_USER* myUser,
        const char*         myPublicIp
        );

    virtual void PRO_CALLTYPE OnRecvMsg(
        IRtpMsgClient*      msgClient,
        const void*         buf,
        unsigned long       size,
        PRO_UINT16          charset,
        const RTP_MSG_USER* srcUser
        );

    virtual void PRO_CALLTYPE OnCloseMsg(
        IRtpMsgClient* msgClient,
        long           errorCode,
        long           sslCode,
        bool           tcpConnected
        );

    virtual void PRO_CALLTYPE OnHeartbeatMsg(
        IRtpMsgClient* msgClient,
        PRO_INT64      peerAliveTick
        );

private:

    IMsgClientObserver* m_observer;

    DECLARE_SGI_POOL(0)
};

/////////////////////////////////////////////////////////////////////////////
////

#endif /* ____MSG_CLIENT2_H____ */
