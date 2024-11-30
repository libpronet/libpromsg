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

#include "msg_client2.h"
#include "msg_client.h"
#include "pronet/pro_memory_pool.h"
#include "pronet/pro_thread_mutex.h"
#include "pronet/pro_time_util.h"
#include "pronet/pro_z.h"
#include "pronet/rtp_base.h"
#include "pronet/rtp_msg.h"

/////////////////////////////////////////////////////////////////////////////
////

CMsgClient2*
CMsgClient2::CreateInstance()
{
    return new CMsgClient2;
}

CMsgClient2::CMsgClient2()
{
    m_observer = NULL;
}

CMsgClient2::~CMsgClient2()
{
    Fini();
}

bool
CMsgClient2::Init(IMsgClientObserver* observer,
                  IProReactor*        reactor,
                  const char*         argv0,      /* = NULL */
                  const char*         configFileName,
                  RTP_MM_TYPE         mmType,     /* = 0 */
                  const char*         serverIp,   /* = NULL */
                  unsigned short      serverPort, /* = 0 */
                  const RTP_MSG_USER* user,       /* = NULL */
                  const char*         password,   /* = NULL */
                  const char*         localIp)    /* = NULL */
{
    assert(observer != NULL);
    if (observer == NULL)
    {
        return false;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        assert(m_observer == NULL);
        if (m_observer != NULL)
        {
            return false;
        }

        if (!CMsgClient::Init(reactor, argv0, configFileName, mmType,
            serverIp, serverPort, user, password, localIp))
        {
            return false;
        }

        observer->AddRef();
        m_observer = observer;
    }

    return true;
}

void
CMsgClient2::Fini()
{
    IMsgClientObserver* observer = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_observer == NULL)
        {
            return;
        }

        observer = m_observer;
        m_observer = NULL;
    }

    observer->Release();

    CMsgClient::Fini();
}

void
CMsgClient2::OnOkMsg(IRtpMsgClient*      msgClient,
                     const RTP_MSG_USER* myUser,
                     const char*         myPublicIp)
{
    assert(msgClient != NULL);
    assert(myUser != NULL);
    assert(myPublicIp != NULL);
    assert(myPublicIp[0] != '\0');
    if (msgClient == NULL || myUser == NULL || myPublicIp == NULL || myPublicIp[0] == '\0')
    {
        return;
    }

    IMsgClientObserver* observer = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_observer == NULL || m_msgClient == NULL)
        {
            return;
        }

        if (msgClient != m_msgClient)
        {
            return;
        }

        m_observer->AddRef();
        observer = m_observer;
    }

    if (0)
    {{{
        char suiteName[64] = "";
        msgClient->GetSslSuite(suiteName);

        CProStlString timeString;
        ProGetLocalTimeString(timeString);

        printf(
            "\n"
            "%s \n"
            " CMsgClient2::OnOkMsg(id : %u-%llu-%u, publicIp : %s,"
            " sslSuite : %s, server : %s:%u) \n"
            ,
            timeString.c_str(),
            (unsigned int)myUser->classId,
            (unsigned long long)myUser->UserId(),
            (unsigned int)myUser->instId,
            myPublicIp,
            suiteName,
            m_msgConfigInfo.msgc_server_ip.c_str(),
            (unsigned int)m_msgConfigInfo.msgc_server_port
            );
    }}}

    observer->OnOkMsg(this, myUser, myPublicIp);
    observer->Release();
}

void
CMsgClient2::OnRecvMsg(IRtpMsgClient*      msgClient,
                       const void*         buf,
                       size_t              size,
                       uint16_t            charset,
                       const RTP_MSG_USER* srcUser)
{
    assert(msgClient != NULL);
    assert(buf != NULL);
    assert(size > 0);
    assert(srcUser != NULL);
    if (msgClient == NULL || buf == NULL || size == 0 || srcUser == NULL)
    {
        return;
    }

    IMsgClientObserver* observer = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_observer == NULL || m_msgClient == NULL)
        {
            return;
        }

        if (msgClient != m_msgClient)
        {
            return;
        }

        m_observer->AddRef();
        observer = m_observer;
    }

    if (0)
    {{{
        CProStlString msg((char*)buf, size);

        RTP_MSG_USER myUser;
        msgClient->GetUser(&myUser);

        CProStlString timeString;
        ProGetLocalTimeString(timeString);

        printf(
            "\n"
            "%s \n"
            " CMsgClient2::OnRecvMsg(from : %u-%llu-%u, me : %u-%llu-%u) \n"
            "\t %s \n"
            ,
            timeString.c_str(),
            (unsigned int)srcUser->classId,
            (unsigned long long)srcUser->UserId(),
            (unsigned int)srcUser->instId,
            (unsigned int)myUser.classId,
            (unsigned long long)myUser.UserId(),
            (unsigned int)myUser.instId,
            msg.c_str()
            );
    }}}

    observer->OnRecvMsg(this, buf, size, charset, srcUser);
    observer->Release();
}

void
CMsgClient2::OnCloseMsg(IRtpMsgClient* msgClient,
                        int            errorCode,
                        int            sslCode,
                        bool           tcpConnected)
{
    assert(msgClient != NULL);
    if (msgClient == NULL)
    {
        return;
    }

    IMsgClientObserver* observer = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_observer == NULL || m_msgClient == NULL)
        {
            return;
        }

        if (msgClient != m_msgClient)
        {
            return;
        }

        m_observer->AddRef();
        observer = m_observer;
    }

    if (0)
    {{{
        RTP_MSG_USER myUser;
        msgClient->GetUser(&myUser);

        CProStlString timeString;
        ProGetLocalTimeString(timeString);

        printf(
            "\n"
            "%s \n"
            " CMsgClient2::OnCloseMsg(id : %u-%llu-%u,"
            " errorCode : [%d, %d], tcpConnected : %d, server : %s:%u) \n"
            ,
            timeString.c_str(),
            (unsigned int)myUser.classId,
            (unsigned long long)myUser.UserId(),
            (unsigned int)myUser.instId,
            errorCode,
            sslCode,
            (int)tcpConnected,
            m_msgConfigInfo.msgc_server_ip.c_str(),
            (unsigned int)m_msgConfigInfo.msgc_server_port
            );
    }}}

    observer->OnCloseMsg(this, errorCode, sslCode, tcpConnected);
    observer->Release();
}

void
CMsgClient2::OnHeartbeatMsg(IRtpMsgClient* msgClient,
                            int64_t        peerAliveTick)
{
    assert(msgClient != NULL);
    if (msgClient == NULL)
    {
        return;
    }

    IMsgClientObserver* observer = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_observer == NULL || m_msgClient == NULL)
        {
            return;
        }

        if (msgClient != m_msgClient)
        {
            return;
        }

        m_observer->AddRef();
        observer = m_observer;
    }

    observer->OnHeartbeatMsg(this, peerAliveTick);
    observer->Release();
}
