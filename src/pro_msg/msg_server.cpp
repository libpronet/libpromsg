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

#include "msg_server.h"
#include "pronet/pro_config_file.h"
#include "pronet/pro_memory_pool.h"
#include "pronet/pro_ref_count.h"
#include "pronet/pro_ssl_util.h"
#include "pronet/pro_stl.h"
#include "pronet/pro_thread_mutex.h"
#include "pronet/pro_z.h"
#include "pronet/rtp_base.h"
#include "pronet/rtp_msg.h"
#include <cassert>

/////////////////////////////////////////////////////////////////////////////
////

static
void
PRO_CALLTYPE
ReadConfig_i(CProStlVector<PRO_CONFIG_ITEM>& configs,
             MSG_SERVER_CONFIG_INFO&         configInfo)
{
    char exeRoot[1024] = "";
    ProGetExeDir_(exeRoot);

    configInfo.msgs_ssl_cafiles.clear();
    configInfo.msgs_ssl_crlfiles.clear();
    configInfo.msgs_ssl_certfiles.clear();

    int       i = 0;
    const int c = (int)configs.size();

    for (; i < c; ++i)
    {
        CProStlString& configName  = configs[i].configName;
        CProStlString& configValue = configs[i].configValue;

        if (stricmp(configName.c_str(), "msgs_mm_type") == 0)
        {
            const int value = atoi(configValue.c_str());
            if (value >= (int)RTP_MMT_MSG_MIN &&
                value <= (int)RTP_MMT_MSG_MAX)
            {
                configInfo.msgs_mm_type = (RTP_MM_TYPE)value;
            }
        }
        else if (stricmp(configName.c_str(), "msgs_hub_port") == 0)
        {
            const int value = atoi(configValue.c_str());
            if (value > 0 && value <= 65535)
            {
                configInfo.msgs_hub_port = (unsigned short)value;
            }
        }
        else if (stricmp(configName.c_str(), "msgs_password_cid1") == 0)
        {
            configInfo.msgs_password_cid1 = configValue;

            if (!configValue.empty())
            {
                ProZeroMemory(&configValue[0], configValue.length());
                configValue = "";
            }
        }
        else if (stricmp(configName.c_str(), "msgs_password_cid2") == 0)
        {
            configInfo.msgs_password_cid2 = configValue;

            if (!configValue.empty())
            {
                ProZeroMemory(&configValue[0], configValue.length());
                configValue = "";
            }
        }
        else if (stricmp(configName.c_str(), "msgs_password_cidx") == 0)
        {
            configInfo.msgs_password_cidx = configValue;

            if (!configValue.empty())
            {
                ProZeroMemory(&configValue[0], configValue.length());
                configValue = "";
            }
        }
        else if (stricmp(configName.c_str(), "msgs_handshake_timeout") == 0)
        {
            const int value = atoi(configValue.c_str());
            if (value > 0)
            {
                configInfo.msgs_handshake_timeout = value;
            }
        }
        else if (stricmp(configName.c_str(), "msgs_redline_bytes") == 0)
        {
            const int value = atoi(configValue.c_str());
            if (value > 0)
            {
                configInfo.msgs_redline_bytes = value;
            }
        }
        else if (stricmp(configName.c_str(), "msgs_enable_ssl") == 0)
        {
            configInfo.msgs_enable_ssl = atoi(configValue.c_str()) != 0;
        }
        else if (stricmp(configName.c_str(), "msgs_ssl_forced") == 0)
        {
            configInfo.msgs_ssl_forced = atoi(configValue.c_str()) != 0;
        }
        else if (stricmp(configName.c_str(), "msgs_ssl_enable_sha1cert") == 0)
        {
            configInfo.msgs_ssl_enable_sha1cert = atoi(configValue.c_str()) != 0;
        }
        else if (stricmp(configName.c_str(), "msgs_ssl_cafile") == 0)
        {
            if (!configValue.empty())
            {
                if (configValue[0] == '.' ||
                    configValue.find_first_of("\\/") == CProStlString::npos)
                {
                    CProStlString fileName = exeRoot;
                    fileName += configValue;
                    configValue = fileName;
                }
            }

            if (!configValue.empty())
            {
                configInfo.msgs_ssl_cafiles.push_back(configValue);
            }
        }
        else if (stricmp(configName.c_str(), "msgs_ssl_crlfile") == 0)
        {
            if (!configValue.empty())
            {
                if (configValue[0] == '.' ||
                    configValue.find_first_of("\\/") == CProStlString::npos)
                {
                    CProStlString fileName = exeRoot;
                    fileName += configValue;
                    configValue = fileName;
                }
            }

            if (!configValue.empty())
            {
                configInfo.msgs_ssl_crlfiles.push_back(configValue);
            }
        }
        else if (stricmp(configName.c_str(), "msgs_ssl_certfile") == 0)
        {
            if (!configValue.empty())
            {
                if (configValue[0] == '.' ||
                    configValue.find_first_of("\\/") == CProStlString::npos)
                {
                    CProStlString fileName = exeRoot;
                    fileName += configValue;
                    configValue = fileName;
                }
            }

            if (!configValue.empty())
            {
                configInfo.msgs_ssl_certfiles.push_back(configValue);
            }
        }
        else if (stricmp(configName.c_str(), "msgs_ssl_keyfile") == 0)
        {
            if (!configValue.empty())
            {
                if (configValue[0] == '.' ||
                    configValue.find_first_of("\\/") == CProStlString::npos)
                {
                    CProStlString fileName = exeRoot;
                    fileName += configValue;
                    configValue = fileName;
                }
            }

            configInfo.msgs_ssl_keyfile = configValue;
        }
        else
        {
        }
    } /* end of for (...) */
}

/////////////////////////////////////////////////////////////////////////////
////

CMsgServer*
CMsgServer::CreateInstance()
{
    CMsgServer* const server = new CMsgServer;

    return (server);
}

CMsgServer::CMsgServer()
{
    m_reactor   = NULL;
    m_sslConfig = NULL;
    m_msgServer = NULL;
}

CMsgServer::~CMsgServer()
{
    Fini();
}

bool
CMsgServer::Init(IProReactor*   reactor,
                 const char*    configFileName,
                 RTP_MM_TYPE    mmType,         /* = 0 */
                 unsigned short serviceHubPort) /* = 0 */
{
    assert(reactor != NULL);
    assert(configFileName != NULL);
    assert(configFileName[0] != '\0');
    if (reactor == NULL ||
        configFileName == NULL || configFileName[0] == '\0')
    {
        return (false);
    }

    char exeRoot[1024] = "";
    ProGetExeDir_(exeRoot);

    CProStlString configFileName2 = configFileName;
    if (configFileName2[0] == '.' ||
        configFileName2.find_first_of("\\/") == CProStlString::npos)
    {
        CProStlString fileName = exeRoot;
        fileName += configFileName2;
        configFileName2 = fileName;
    }

    CProConfigFile configFile;
    configFile.Init(configFileName2.c_str());

    CProStlVector<PRO_CONFIG_ITEM> configs;
    if (!configFile.Read(configs))
    {
        return (false);
    }

    MSG_SERVER_CONFIG_INFO configInfo;
    ReadConfig_i(configs, configInfo);

    /*
     * override
     */
    if (mmType >= RTP_MMT_MSG_MIN && mmType <= RTP_MMT_MSG_MAX)
    {
        configInfo.msgs_mm_type  = mmType;
    }
    if (serviceHubPort > 0)
    {
        configInfo.msgs_hub_port = serviceHubPort;
    }

    PRO_SSL_SERVER_CONFIG* sslConfig = NULL;
    IRtpMsgServer*         msgServer = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        assert(m_reactor == NULL);
        assert(m_sslConfig == NULL);
        assert(m_msgServer == NULL);
        if (m_reactor != NULL || m_sslConfig != NULL || m_msgServer != NULL)
        {
            return (false);
        }

        if (configInfo.msgs_enable_ssl)
        {
            CProStlVector<const char*> caFiles;
            CProStlVector<const char*> crlFiles;
            CProStlVector<const char*> certFiles;

            int i = 0;
            int c = (int)configInfo.msgs_ssl_cafiles.size();

            for (; i < c; ++i)
            {
                if (!configInfo.msgs_ssl_cafiles[i].empty())
                {
                    caFiles.push_back(&configInfo.msgs_ssl_cafiles[i][0]);
                }
            }

            i = 0;
            c = (int)configInfo.msgs_ssl_crlfiles.size();

            for (; i < c; ++i)
            {
                if (!configInfo.msgs_ssl_crlfiles[i].empty())
                {
                    crlFiles.push_back(&configInfo.msgs_ssl_crlfiles[i][0]);
                }
            }

            i = 0;
            c = (int)configInfo.msgs_ssl_certfiles.size();

            for (; i < c; ++i)
            {
                if (!configInfo.msgs_ssl_certfiles[i].empty())
                {
                    certFiles.push_back(&configInfo.msgs_ssl_certfiles[i][0]);
                }
            }

            if (caFiles.size() > 0 && certFiles.size() > 0)
            {
                sslConfig = ProSslServerConfig_Create();
                if (sslConfig == NULL)
                {
                    goto EXIT;
                }

                ProSslServerConfig_EnableSha1Cert(
                    sslConfig, configInfo.msgs_ssl_enable_sha1cert);

                if (!ProSslServerConfig_SetCaList(
                    sslConfig,
                    &caFiles[0],
                    caFiles.size(),
                    crlFiles.size() > 0 ? &crlFiles[0] : NULL,
                    crlFiles.size()
                    ))
                {
                    goto EXIT;
                }

                if (!ProSslServerConfig_AppendCertChain(
                    sslConfig,
                    &certFiles[0],
                    certFiles.size(),
                    configInfo.msgs_ssl_keyfile.c_str(),
                    NULL
                    ))
                {
                    goto EXIT;
                }
            }
        }

        msgServer = CreateRtpMsgServer(
            this,
            reactor,
            configInfo.msgs_mm_type,
            sslConfig,
            configInfo.msgs_ssl_forced,
            configInfo.msgs_hub_port,
            configInfo.msgs_handshake_timeout
            );
        if (msgServer == NULL)
        {
            goto EXIT;
        }

        msgServer->SetOutputRedlineToUsr(configInfo.msgs_redline_bytes);

        m_reactor       = reactor;
        m_msgConfigInfo = configInfo;
        m_sslConfig     = sslConfig;
        m_msgServer     = msgServer;
    }

    return (true);

EXIT:

    DeleteRtpMsgServer(msgServer);
    ProSslServerConfig_Delete(sslConfig);

    return (false);
}

void
CMsgServer::Fini()
{
    PRO_SSL_SERVER_CONFIG* sslConfig = NULL;
    IRtpMsgServer*         msgServer = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL)
        {
            return;
        }

        msgServer = m_msgServer;
        m_msgServer = NULL;
        sslConfig = m_sslConfig;
        m_sslConfig = NULL;
        m_reactor = NULL;
    }

    DeleteRtpMsgServer(msgServer);
    ProSslServerConfig_Delete(sslConfig);
}

unsigned long
PRO_CALLTYPE
CMsgServer::AddRef()
{
    const unsigned long refCount = CProRefCount::AddRef();

    return (refCount);
}

unsigned long
PRO_CALLTYPE
CMsgServer::Release()
{
    const unsigned long refCount = CProRefCount::Release();

    return (refCount);
}

RTP_MM_TYPE
CMsgServer::GetMmType() const
{
    RTP_MM_TYPE mmType = 0;

    {
        CProThreadMutexGuard mon(m_lock);

        mmType = m_msgConfigInfo.msgs_mm_type;
    }

    return (mmType);
}

unsigned short
CMsgServer::GetServicePort() const
{
    unsigned short servicePort = 0;

    {
        CProThreadMutexGuard mon(m_lock);

        servicePort = m_msgConfigInfo.msgs_hub_port;
    }

    return (servicePort);
}

const char*
CMsgServer::GetSslSuite(const RTP_MSG_USER& user,
                        char                suiteName[64]) const
{
    strcpy(suiteName, "NONE");

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_msgServer != NULL)
        {
            m_msgServer->GetSslSuite(&user, suiteName);
        }
    }

    return (suiteName);
}

unsigned long
CMsgServer::GetUserCount() const
{
    unsigned long baseUserCount = 0;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_msgServer != NULL)
        {
            m_msgServer->GetUserCount(NULL, &baseUserCount, NULL);
        }
    }

    return (baseUserCount);
}

void
CMsgServer::KickoutUser(const RTP_MSG_USER& user)
{
    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgServer == NULL)
        {
            return;
        }

        m_msgServer->KickoutUser(&user);
    }
}

bool
CMsgServer::SendMsg(const void*         buf,
                    unsigned long       size,
                    PRO_UINT16          charset,
                    const RTP_MSG_USER* dstUsers,
                    unsigned char       dstUserCount)
{
    const bool ret = SendMsg2(
        buf, size, NULL, 0, charset, dstUsers, dstUserCount);

    return (ret);
}

bool
CMsgServer::SendMsg2(const void*         buf1,
                     unsigned long       size1,
                     const void*         buf2,  /* = NULL */
                     unsigned long       size2, /* = 0 */
                     PRO_UINT16          charset,
                     const RTP_MSG_USER* dstUsers,
                     unsigned char       dstUserCount)
{
    IRtpMsgServer* msgServer = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgServer == NULL)
        {
            return (false);
        }

        m_msgServer->AddRef();
        msgServer = m_msgServer;
    }

    const bool ret = msgServer->SendMsg2(
        buf1, size1, buf2, size2, charset, dstUsers, dstUserCount);
    msgServer->Release();

    return (ret);
}

void
CMsgServer::SetOutputRedline(unsigned long redlineBytes)
{
    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgServer == NULL)
        {
            return;
        }

        m_msgServer->SetOutputRedlineToUsr(redlineBytes);
        m_msgConfigInfo.msgs_redline_bytes =
            m_msgServer->GetOutputRedlineToUsr();
    }
}

unsigned long
CMsgServer::GetOutputRedline() const
{
    unsigned long redlineBytes = 0;

    {
        CProThreadMutexGuard mon(m_lock);

        redlineBytes = m_msgConfigInfo.msgs_redline_bytes;
    }

    return (redlineBytes);
}

unsigned long
CMsgServer::GetSendingBytes(const RTP_MSG_USER& user) const
{
    unsigned long sendingBytes = 0;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_msgServer != NULL)
        {
            sendingBytes = m_msgServer->GetSendingBytes(&user);
        }
    }

    return (sendingBytes);
}

bool
PRO_CALLTYPE
CMsgServer::OnCheckUser(IRtpMsgServer*      msgServer,
                        const RTP_MSG_USER* user,
                        const char*         userPublicIp,
                        const RTP_MSG_USER* c2sUser, /* = NULL */
                        const char          hash[32],
                        const char          nonce[32],
                        PRO_UINT64*         userId,
                        PRO_UINT16*         instId,
                        PRO_INT64*          appData,
                        bool*               isC2s)
{
    assert(msgServer != NULL);
    assert(user != NULL);
    assert(user->classId > 0);
    assert(user->UserId() > 0);
    assert(userPublicIp != NULL);
    assert(userPublicIp[0] != '\0');
    assert(userId != NULL);
    assert(instId != NULL);
    assert(appData != NULL);
    assert(isC2s != NULL);
    if (msgServer == NULL || user == NULL ||
        user->classId == 0 || user->UserId() == 0 ||
        userPublicIp == NULL || userPublicIp[0] == '\0' ||
        userId == NULL || instId == NULL || appData == NULL || isC2s == NULL)
    {
        return (false);
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgServer == NULL)
        {
            return (false);
        }

        if (msgServer != m_msgServer)
        {
            return (false);
        }

        const char* password = NULL;

        if (user->classId == 1)      /* 1-... */
        {
            password = &m_msgConfigInfo.msgs_password_cid1[0];
        }
        else if (user->classId == 2) /* 2-... */
        {
            password = &m_msgConfigInfo.msgs_password_cid2[0];
        }
        else                         /* others */
        {
            password = &m_msgConfigInfo.msgs_password_cidx[0];
        }

        if (!CheckRtpServiceData(nonce, password, hash))
        {
            return (false);
        }

        *userId  = user->UserId();
        *instId  = user->instId;
        *appData = 0; /* You can do something. */
        *isC2s   = false;
    }

    return (true);
}

void
PRO_CALLTYPE
CMsgServer::OnOkUser(IRtpMsgServer*      msgServer,
                     const RTP_MSG_USER* user,
                     const char*         userPublicIp,
                     const RTP_MSG_USER* c2sUser, /* = NULL */
                     PRO_INT64           appData)
{
    assert(msgServer != NULL);
    assert(user != NULL);
    assert(userPublicIp != NULL);
    assert(userPublicIp[0] != '\0');
    if (msgServer == NULL || user == NULL || userPublicIp == NULL ||
        userPublicIp[0] == '\0')
    {
        return;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgServer == NULL)
        {
            return;
        }

        if (msgServer != m_msgServer)
        {
            return;
        }

        /*
         * ...
         */
    }
}

void
PRO_CALLTYPE
CMsgServer::OnCloseUser(IRtpMsgServer*      msgServer,
                        const RTP_MSG_USER* user,
                        long                errorCode,
                        long                sslCode)
{
    assert(msgServer != NULL);
    assert(user != NULL);
    if (msgServer == NULL || user == NULL)
    {
        return;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgServer == NULL)
        {
            return;
        }

        if (msgServer != m_msgServer)
        {
            return;
        }

        /*
         * ...
         */
    }
}

void
PRO_CALLTYPE
CMsgServer::OnRecvMsg(IRtpMsgServer*      msgServer,
                      const void*         buf,
                      unsigned long       size,
                      PRO_UINT16          charset,
                      const RTP_MSG_USER* srcUser)
{
    assert(msgServer != NULL);
    assert(buf != NULL);
    assert(size > 0);
    assert(srcUser != NULL);
    if (msgServer == NULL || buf == NULL || size == 0 || srcUser == NULL)
    {
        return;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgServer == NULL)
        {
            return;
        }

        if (msgServer != m_msgServer)
        {
            return;
        }

        /*
         * ...
         */
    }
}
