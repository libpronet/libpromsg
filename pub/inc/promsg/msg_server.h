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

#if !defined(____MSG_SERVER_H____)
#define ____MSG_SERVER_H____

#include "pronet/pro_memory_pool.h"
#include "pronet/pro_ref_count.h"
#include "pronet/pro_ssl_util.h"
#include "pronet/pro_stl.h"
#include "pronet/pro_thread_mutex.h"
#include "pronet/rtp_base.h"
#include "pronet/rtp_msg.h"

/////////////////////////////////////////////////////////////////////////////
////

struct MSG_SERVER_CONFIG_INFO
{
    MSG_SERVER_CONFIG_INFO()
    {
        msgs_mm_type             = RTP_MMT_MSG;
        msgs_hub_port            = 3000;
        msgs_password_cid1       = "test";
        msgs_password_cid2       = "test";
        msgs_password_cidx       = "test";
        msgs_handshake_timeout   = 20;
        msgs_redline_bytes       = 1024000;

        msgs_enable_ssl          = true;
        msgs_ssl_forced          = false;
        msgs_ssl_enable_sha1cert = true;
        msgs_ssl_keyfile         = "./server.key";

        msgs_ssl_cafiles.push_back("./ca.crt");
        msgs_ssl_cafiles.push_back("");
        msgs_ssl_crlfiles.push_back("");
        msgs_ssl_crlfiles.push_back("");
        msgs_ssl_certfiles.push_back("./server.crt");
        msgs_ssl_certfiles.push_back("");
    }

    ~MSG_SERVER_CONFIG_INFO()
    {
        if (!msgs_password_cid1.empty())
        {
            ProZeroMemory(&msgs_password_cid1[0], msgs_password_cid1.length());
        }
        if (!msgs_password_cid2.empty())
        {
            ProZeroMemory(&msgs_password_cid2[0], msgs_password_cid2.length());
        }
        if (!msgs_password_cidx.empty())
        {
            ProZeroMemory(&msgs_password_cidx[0], msgs_password_cidx.length());
        }

        msgs_password_cid1 = "";
        msgs_password_cid2 = "";
        msgs_password_cidx = "";
    }

    RTP_MM_TYPE                  msgs_mm_type;       /* RTP_MMT_MSG_MIN ~ RTP_MMT_MSG_MAX */
    unsigned short               msgs_hub_port;
    CProStlString                msgs_password_cid1; /* for 1-... */
    CProStlString                msgs_password_cid2; /* for 2-... */
    CProStlString                msgs_password_cidx; /* for x-... */
    unsigned int                 msgs_handshake_timeout;
    unsigned int                 msgs_redline_bytes;

    bool                         msgs_enable_ssl;
    bool                         msgs_ssl_forced;
    bool                         msgs_ssl_enable_sha1cert;
    CProStlVector<CProStlString> msgs_ssl_cafiles;
    CProStlVector<CProStlString> msgs_ssl_crlfiles;
    CProStlVector<CProStlString> msgs_ssl_certfiles;
    CProStlString                msgs_ssl_keyfile;

    DECLARE_SGI_POOL(0)
};

/////////////////////////////////////////////////////////////////////////////
////

class CMsgServer : public IRtpMsgServerObserver, public CProRefCount
{
public:

    static CMsgServer* CreateInstance();

    bool Init(
        IProReactor*   reactor,
        const char*    configFileName,
        RTP_MM_TYPE    mmType,        /* = 0 */
        unsigned short serviceHubPort /* = 0 */
        );

    void Fini();

    virtual unsigned long AddRef();

    virtual unsigned long Release();

    RTP_MM_TYPE GetMmType() const;

    unsigned short GetServicePort() const;

    const char* GetSslSuite(
        const RTP_MSG_USER& user,
        char                suiteName[64]
        ) const;

    unsigned long GetUserCount() const;

    void KickoutUser(const RTP_MSG_USER& user);

    bool SendMsg(
        const void*         buf,
        unsigned long       size,
        PRO_UINT16          charset,
        const RTP_MSG_USER* dstUsers,
        unsigned char       dstUserCount
        );

    bool SendMsg2(
        const void*         buf1,
        unsigned long       size1,
        const void*         buf2,  /* = NULL */
        unsigned long       size2, /* = 0 */
        PRO_UINT16          charset,
        const RTP_MSG_USER* dstUsers,
        unsigned char       dstUserCount
        );

    void SetOutputRedline(unsigned long redlineBytes);

    unsigned long GetOutputRedline() const;

    unsigned long GetSendingBytes(const RTP_MSG_USER& user) const;

protected:

    CMsgServer();

    virtual ~CMsgServer();

    virtual bool OnCheckUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* user,
        const char*         userPublicIp,
        const RTP_MSG_USER* c2sUser, /* = NULL */
        const char          hash[32],
        const char          nonce[32],
        PRO_UINT64*         userId,
        PRO_UINT16*         instId,
        PRO_INT64*          appData,
        bool*               isC2s
        );

    virtual void OnOkUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* user,
        const char*         userPublicIp,
        const RTP_MSG_USER* c2sUser, /* = NULL */
        PRO_INT64           appData
        );

    virtual void OnCloseUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* user,
        long                errorCode,
        long                sslCode
        );

    virtual void OnHeartbeatUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* user,
        PRO_INT64           peerAliveTick
        );

    virtual void OnRecvMsg(
        IRtpMsgServer*      msgServer,
        const void*         buf,
        unsigned long       size,
        PRO_UINT16          charset,
        const RTP_MSG_USER* srcUser
        );

protected:

    IProReactor*                     m_reactor;
    MSG_SERVER_CONFIG_INFO           m_msgConfigInfo;
    PRO_SSL_SERVER_CONFIG*           m_sslConfig;
    IRtpMsgServer*                   m_msgServer;
    mutable CProRecursiveThreadMutex m_lock;

    DECLARE_SGI_POOL(0)
};

/////////////////////////////////////////////////////////////////////////////
////

#endif /* ____MSG_SERVER_H____ */
