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

#if !defined(MSG_CLIENT_H)
#define MSG_CLIENT_H

#include "pro/pro_memory_pool.h"
#include "pro/pro_ref_count.h"
#include "pro/pro_ssl_util.h"
#include "pro/pro_stl.h"
#include "pro/pro_thread_mutex.h"
#include "pro/rtp_base.h"
#include "pro/rtp_msg.h"

/////////////////////////////////////////////////////////////////////////////
////

struct MSG_CLIENT_CONFIG_INFO
{
    MSG_CLIENT_CONFIG_INFO()
    {
        msgc_server_ip           = "127.0.0.1";
        msgc_server_port         = 3000;
        msgc_password            = "test";
        msgc_local_ip            = "0.0.0.0";
        msgc_handshake_timeout   = 20;
        msgc_redline_bytes       = 1024000;
        msgc_mm_type             = RTP_MMT_MSG;

        msgc_enable_ssl          = false;
        msgc_ssl_enable_sha1cert = true;
        msgc_ssl_sni             = "server.libpro.org";
        msgc_ssl_aes256          = false;

        RtpMsgString2User("2-0-0", &msgc_id);

        msgc_ssl_cafiles.push_back("./ca.crt");
        msgc_ssl_cafiles.push_back("");
        msgc_ssl_crlfiles.push_back("");
        msgc_ssl_crlfiles.push_back("");
    }

    ~MSG_CLIENT_CONFIG_INFO()
    {
        if (!msgc_password.empty())
        {
            ProZeroMemory(&msgc_password[0], msgc_password.length());
            msgc_password = "";
        }
    }

    CProStlString                msgc_server_ip;
    unsigned short               msgc_server_port;
    RTP_MSG_USER                 msgc_id;
    CProStlString                msgc_password;
    CProStlString                msgc_local_ip;
    unsigned int                 msgc_handshake_timeout;
    unsigned int                 msgc_redline_bytes;
    RTP_MM_TYPE                  msgc_mm_type; /* RTP_MMT_MSG_MIN ~ RTP_MMT_MSG_MAX */

    bool                         msgc_enable_ssl;
    bool                         msgc_ssl_enable_sha1cert;
    CProStlVector<CProStlString> msgc_ssl_cafiles;
    CProStlVector<CProStlString> msgc_ssl_crlfiles;
    CProStlString                msgc_ssl_sni;
    bool                         msgc_ssl_aes256;

    DECLARE_SGI_POOL(0);
};

/////////////////////////////////////////////////////////////////////////////
////

class CMsgClient : public IRtpMsgClientObserver, public CProRefCount
{
public:

    static CMsgClient* CreateInstance();

    bool Init(
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

    virtual unsigned long PRO_CALLTYPE AddRef();

    virtual unsigned long PRO_CALLTYPE Release();

    void GetUser(RTP_MSG_USER& user) const;

    const char* GetSslSuite(char suiteName[64]) const;

    const char* GetLocalIp(char localIp[64]) const;

    unsigned short GetLocalPort() const;

    const char* GetRemoteIp(char remoteIp[64]) const;

    unsigned short GetRemotePort() const;

    bool SendMsg(
        const void*         buf,
        unsigned long       size,
        PRO_UINT16          charset,
        const RTP_MSG_USER* dstUsers,
        unsigned char       dstUserCount
        );

    void SetOutputRedline(unsigned long redlineBytes);

    unsigned long GetOutputRedline() const;

    bool Reconnect();

protected:

    CMsgClient();

    virtual ~CMsgClient();

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

protected:

    IProReactor*                     m_reactor;
    MSG_CLIENT_CONFIG_INFO           m_msgConfigInfo;
    PRO_SSL_CLIENT_CONFIG*           m_sslConfig;
    IRtpMsgClient*                   m_msgClient;
    mutable CProRecursiveThreadMutex m_lock;

    DECLARE_SGI_POOL(0);
};

/////////////////////////////////////////////////////////////////////////////
////

#endif /* MSG_CLIENT_H */
