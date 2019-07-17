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

#include "msg_client.h"
#include "pro/pro_bsd_wrapper.h"
#include "pro/pro_config_file.h"
#include "pro/pro_memory_pool.h"
#include "pro/pro_ref_count.h"
#include "pro/pro_ssl_util.h"
#include "pro/pro_stl.h"
#include "pro/pro_thread_mutex.h"
#include "pro/pro_z.h"
#include "pro/rtp_base.h"
#include "pro/rtp_msg.h"
#include <cassert>

/////////////////////////////////////////////////////////////////////////////
////

static
void
PRO_CALLTYPE
ReadConfig_i(CProStlVector<PRO_CONFIG_ITEM>& configs,
             MSG_CLIENT_CONFIG_INFO&         configInfo)
{
    char exeRoot[1024] = "";
    ProGetExeDir_(exeRoot);

    configInfo.msgc_ssl_cafiles.clear();
    configInfo.msgc_ssl_crlfiles.clear();

    int       i = 0;
    const int c = (int)configs.size();

    for (; i < c; ++i)
    {
        CProStlString& configName  = configs[i].configName;
        CProStlString& configValue = configs[i].configValue;

        if (stricmp(configName.c_str(), "msgc_mm_type") == 0)
        {
            const int value = atoi(configValue.c_str());
            if (value >= (int)RTP_MMT_MSG_MIN && value <= (int)RTP_MMT_MSG_MAX)
            {
                configInfo.msgc_mm_type = (RTP_MM_TYPE)value;
            }
        }
        else if (stricmp(configName.c_str(), "msgc_server_ip") == 0)
        {
            if (!configValue.empty())
            {
                configInfo.msgc_server_ip = configValue;
            }
        }
        else if (stricmp(configName.c_str(), "msgc_server_port") == 0)
        {
            const int value = atoi(configValue.c_str());
            if (value > 0 && value <= 65535)
            {
                configInfo.msgc_server_port = (unsigned short)value;
            }
        }
        else if (stricmp(configName.c_str(), "msgc_id") == 0)
        {
            if (!configValue.empty())
            {
                RtpMsgString2User(configValue.c_str(), &configInfo.msgc_id);
            }
        }
        else if (stricmp(configName.c_str(), "msgc_password") == 0)
        {
            configInfo.msgc_password = configValue;

            if (!configValue.empty())
            {
                ProZeroMemory(&configValue[0], configValue.length());
                configValue = "";
            }
        }
        else if (stricmp(configName.c_str(), "msgc_local_ip") == 0)
        {
            if (!configValue.empty())
            {
                configInfo.msgc_local_ip = configValue;
            }
        }
        else if (stricmp(configName.c_str(), "msgc_handshake_timeout") == 0)
        {
            const int value = atoi(configValue.c_str());
            if (value > 0)
            {
                configInfo.msgc_handshake_timeout = value;
            }
        }
        else if (stricmp(configName.c_str(), "msgc_redline_bytes") == 0)
        {
            const int value = atoi(configValue.c_str());
            if (value > 0)
            {
                configInfo.msgc_redline_bytes = value;
            }
        }
        else if (stricmp(configName.c_str(), "msgc_enable_ssl") == 0)
        {
            configInfo.msgc_enable_ssl = atoi(configValue.c_str()) != 0;
        }
        else if (stricmp(configName.c_str(), "msgc_ssl_enable_sha1cert") == 0)
        {
            configInfo.msgc_ssl_enable_sha1cert = atoi(configValue.c_str()) != 0;
        }
        else if (stricmp(configName.c_str(), "msgc_ssl_cafile") == 0)
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
                configInfo.msgc_ssl_cafiles.push_back(configValue);
            }
        }
        else if (stricmp(configName.c_str(), "msgc_ssl_crlfile") == 0)
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
                configInfo.msgc_ssl_crlfiles.push_back(configValue);
            }
        }
        else if (stricmp(configName.c_str(), "msgc_ssl_sni") == 0)
        {
            configInfo.msgc_ssl_sni = configValue;
        }
        else if (stricmp(configName.c_str(), "msgc_ssl_aes256") == 0)
        {
            configInfo.msgc_ssl_aes256 = atoi(configValue.c_str()) != 0;
        }
        else
        {
        }
    } /* end of for (...) */
}

/////////////////////////////////////////////////////////////////////////////
////

CMsgClient*
CMsgClient::CreateInstance()
{
    CMsgClient* const client = new CMsgClient;

    return (client);
}

CMsgClient::CMsgClient()
{
    m_reactor   = NULL;
    m_sslConfig = NULL;
    m_msgClient = NULL;
}

CMsgClient::~CMsgClient()
{
    Fini();
}

bool
CMsgClient::Init(IProReactor*        reactor,
                 const char*         configFileName,
                 RTP_MM_TYPE         mmType,     /* = 0 */
                 const char*         serverIp,   /* = NULL */
                 unsigned short      serverPort, /* = 0 */
                 const RTP_MSG_USER* user,       /* = NULL */
                 const char*         password,   /* = NULL */
                 const char*         localIp)    /* = NULL */
{
    assert(reactor != NULL);
    assert(configFileName != NULL);
    assert(configFileName[0] != '\0');
    if (reactor == NULL || configFileName == NULL || configFileName[0] == '\0')
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

    MSG_CLIENT_CONFIG_INFO configInfo;
    ReadConfig_i(configs, configInfo);

    /*
     * override
     */
    if (mmType >= RTP_MMT_MSG_MIN && mmType <= RTP_MMT_MSG_MAX)
    {
        configInfo.msgc_mm_type     = mmType;
    }
    if (serverIp != NULL && serverIp[0] != '\0')
    {
        configInfo.msgc_server_ip   = serverIp;
    }
    if (serverPort > 0)
    {
        configInfo.msgc_server_port = serverPort;
    }
    if (user != NULL && user->classId > 0)
    {
        configInfo.msgc_id          = *user;
    }
    if (password != NULL && password[0] != '\0')
    {
        configInfo.msgc_password    = password;
    }
    if (localIp != NULL && localIp[0] != '\0')
    {
        configInfo.msgc_local_ip    = localIp;
    }

    /*
     * DNS, for reconnecting
     */
    {
        const PRO_UINT32 serverIp2 = pbsd_inet_aton(configInfo.msgc_server_ip.c_str());
        if (serverIp2 == (PRO_UINT32)-1 || serverIp2 == 0)
        {
            return (false);
        }

        char serverIpByDNS[64] = "";
        pbsd_inet_ntoa(serverIp2, serverIpByDNS);

        configInfo.msgc_server_ip = serverIpByDNS;
    }

    PRO_SSL_CLIENT_CONFIG* sslConfig = NULL;
    IRtpMsgClient*         msgClient = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        assert(m_reactor == NULL);
        assert(m_sslConfig == NULL);
        assert(m_msgClient == NULL);
        if (m_reactor != NULL || m_sslConfig != NULL || m_msgClient != NULL)
        {
            return (false);
        }

        if (configInfo.msgc_enable_ssl)
        {
            CProStlVector<const char*>      caFiles;
            CProStlVector<const char*>      crlFiles;
            CProStlVector<PRO_SSL_SUITE_ID> suites;

            int i = 0;
            int c = (int)configInfo.msgc_ssl_cafiles.size();

            for (; i < c; ++i)
            {
                if (!configInfo.msgc_ssl_cafiles[i].empty())
                {
                    caFiles.push_back(&configInfo.msgc_ssl_cafiles[i][0]);
                }
            }

            i = 0;
            c = (int)configInfo.msgc_ssl_crlfiles.size();

            for (; i < c; ++i)
            {
                if (!configInfo.msgc_ssl_crlfiles[i].empty())
                {
                    crlFiles.push_back(&configInfo.msgc_ssl_crlfiles[i][0]);
                }
            }

            if (configInfo.msgc_ssl_aes256)
            {
                suites.push_back(PRO_SSL_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384);
                suites.push_back(PRO_SSL_ECDHE_RSA_WITH_AES_256_GCM_SHA384);
                suites.push_back(PRO_SSL_DHE_RSA_WITH_AES_256_GCM_SHA384);
            }
            else
            {
                suites.push_back(PRO_SSL_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256);
                suites.push_back(PRO_SSL_ECDHE_RSA_WITH_AES_128_GCM_SHA256);
                suites.push_back(PRO_SSL_DHE_RSA_WITH_AES_128_GCM_SHA256);
            }

            if (caFiles.size() > 0)
            {
                sslConfig = ProSslClientConfig_Create();
                if (sslConfig == NULL)
                {
                    goto EXIT;
                }

                ProSslClientConfig_EnableSha1Cert(sslConfig, configInfo.msgc_ssl_enable_sha1cert);

                if (!ProSslClientConfig_SetCaList(
                    sslConfig,
                    &caFiles[0],
                    caFiles.size(),
                    crlFiles.size() > 0 ? &crlFiles[0] : NULL,
                    crlFiles.size()
                    ))
                {
                    goto EXIT;
                }

                if (!ProSslClientConfig_SetSuiteList(sslConfig, &suites[0], suites.size()))
                {
                    goto EXIT;
                }
            }
        }

        msgClient = CreateRtpMsgClient(
            this,
            reactor,
            configInfo.msgc_mm_type,
            sslConfig,
            configInfo.msgc_ssl_sni.c_str(),
            configInfo.msgc_server_ip.c_str(),
            configInfo.msgc_server_port,
            &configInfo.msgc_id,
            configInfo.msgc_password.c_str(),
            configInfo.msgc_local_ip.c_str(),
            configInfo.msgc_handshake_timeout
            );
        if (msgClient == NULL)
        {
            goto EXIT;
        }

        msgClient->SetOutputRedline(configInfo.msgc_redline_bytes);

        m_reactor       = reactor;
        m_msgConfigInfo = configInfo;
        m_sslConfig     = sslConfig;
        m_msgClient     = msgClient;
    }

    return (true);

EXIT:

    DeleteRtpMsgClient(msgClient);
    ProSslClientConfig_Delete(sslConfig);

    return (false);
}

void
CMsgClient::Fini()
{
    PRO_SSL_CLIENT_CONFIG* sslConfig = NULL;
    IRtpMsgClient*         msgClient = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL)
        {
            return;
        }

        msgClient = m_msgClient;
        m_msgClient = NULL;
        sslConfig = m_sslConfig;
        m_sslConfig = NULL;
        m_reactor = NULL;
    }

    DeleteRtpMsgClient(msgClient);
    ProSslClientConfig_Delete(sslConfig);
}

unsigned long
PRO_CALLTYPE
CMsgClient::AddRef()
{
    const unsigned long refCount = CProRefCount::AddRef();

    return (refCount);
}

unsigned long
PRO_CALLTYPE
CMsgClient::Release()
{
    const unsigned long refCount = CProRefCount::Release();

    return (refCount);
}

void
CMsgClient::GetUser(RTP_MSG_USER& user) const
{
    user.Zero();

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_msgClient != NULL)
        {
            m_msgClient->GetUser(&user);
        }
    }
}

const char*
CMsgClient::GetSslSuite(char suiteName[64]) const
{
    strcpy(suiteName, "NONE");

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_msgClient != NULL)
        {
            m_msgClient->GetSslSuite(suiteName);
        }
    }

    return (suiteName);
}

const char*
CMsgClient::GetLocalIp(char localIp[64]) const
{
    strcpy(localIp, "0.0.0.0");

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_msgClient != NULL)
        {
            m_msgClient->GetLocalIp(localIp);
        }
    }

    return (localIp);
}

unsigned short
CMsgClient::GetLocalPort() const
{
    unsigned short localPort = 0;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_msgClient != NULL)
        {
            localPort = m_msgClient->GetLocalPort();
        }
    }

    return (localPort);
}

const char*
CMsgClient::GetRemoteIp(char remoteIp[64]) const
{
    {
        CProThreadMutexGuard mon(m_lock);

        strncpy_pro(remoteIp, 64, m_msgConfigInfo.msgc_server_ip.c_str());
    }

    return (remoteIp);
}

unsigned short
CMsgClient::GetRemotePort() const
{
    unsigned short remotePort = 0;

    {
        CProThreadMutexGuard mon(m_lock);

        remotePort = m_msgConfigInfo.msgc_server_port;
    }

    return (remotePort);
}

bool
CMsgClient::SendMsg(const void*         buf,
                    unsigned long       size,
                    PRO_UINT16          charset,
                    const RTP_MSG_USER* dstUsers,
                    unsigned char       dstUserCount)
{
    IRtpMsgClient* msgClient = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgClient == NULL)
        {
            return (false);
        }

        m_msgClient->AddRef();
        msgClient = m_msgClient;
    }

    const bool ret = msgClient->SendMsg(buf, size, charset, dstUsers, dstUserCount);

    return (ret);
}

void
CMsgClient::SetOutputRedline(unsigned long redlineBytes)
{
    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgClient == NULL)
        {
            return;
        }

        m_msgClient->SetOutputRedline(redlineBytes);
        m_msgConfigInfo.msgc_redline_bytes = m_msgClient->GetOutputRedline();
    }
}

unsigned long
CMsgClient::GetOutputRedline() const
{
    unsigned long redlineBytes = 0;

    {
        CProThreadMutexGuard mon(m_lock);

        redlineBytes = m_msgConfigInfo.msgc_redline_bytes;
    }

    return (redlineBytes);
}

bool
CMsgClient::Reconnect()
{
    IRtpMsgClient* oldMsgClient = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL)
        {
            return (false);
        }

        IRtpMsgClient* const msgClient = CreateRtpMsgClient(
            this,
            m_reactor,
            m_msgConfigInfo.msgc_mm_type,
            m_sslConfig,
            m_msgConfigInfo.msgc_ssl_sni.c_str(),
            m_msgConfigInfo.msgc_server_ip.c_str(),
            m_msgConfigInfo.msgc_server_port,
            &m_msgConfigInfo.msgc_id,
            m_msgConfigInfo.msgc_password.c_str(),
            m_msgConfigInfo.msgc_local_ip.c_str(),
            m_msgConfigInfo.msgc_handshake_timeout
            );
        if (msgClient == NULL)
        {
            return (false);
        }

        msgClient->SetOutputRedline(m_msgConfigInfo.msgc_redline_bytes);

        oldMsgClient = m_msgClient;
        m_msgClient  = msgClient;
    }

    DeleteRtpMsgClient(oldMsgClient);

    return (true);
}

void
PRO_CALLTYPE
CMsgClient::OnOkMsg(IRtpMsgClient*      msgClient,
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

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgClient == NULL)
        {
            return;
        }

        if (msgClient != m_msgClient)
        {
            return;
        }
    }

    {{{
        char suiteName[64] = "";
        msgClient->GetSslSuite(suiteName);

        printf(
            "\n"
            " CMsgClient::OnOkMsg(id : %u-" PRO_PRT64U "-%u, publicIp : %s, sslSuite : %s,"
            " server : %s:%u) \n"
            ,
            (unsigned int)myUser->classId,
            myUser->UserId(),
            (unsigned int)myUser->instId,
            myPublicIp,
            suiteName,
            m_msgConfigInfo.msgc_server_ip.c_str(),
            (unsigned int)m_msgConfigInfo.msgc_server_port
            );
    }}}
}

void
PRO_CALLTYPE
CMsgClient::OnRecvMsg(IRtpMsgClient*      msgClient,
                      const void*         buf,
                      unsigned long       size,
                      PRO_UINT16          charset,
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

    const CProStlString msg((char*)buf, size);

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgClient == NULL)
        {
            return;
        }

        if (msgClient != m_msgClient)
        {
            return;
        }
    }

    {{{
        RTP_MSG_USER myUser;
        msgClient->GetUser(&myUser);

        printf(
            "\n"
            " CMsgClient::OnRecvMsg(from : %u-" PRO_PRT64U "-%u, me : %u-" PRO_PRT64U "-%u) \n"
            "\t %s \n"
            ,
            (unsigned int)srcUser->classId,
            srcUser->UserId(),
            (unsigned int)srcUser->instId,
            (unsigned int)myUser.classId,
            myUser.UserId(),
            (unsigned int)myUser.instId,
            msg.c_str()
            );
    }}}
}

void
PRO_CALLTYPE
CMsgClient::OnCloseMsg(IRtpMsgClient* msgClient,
                       long           errorCode,
                       long           sslCode,
                       bool           tcpConnected)
{
    assert(msgClient != NULL);
    if (msgClient == NULL)
    {
        return;
    }

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_reactor == NULL || m_msgClient == NULL)
        {
            return;
        }

        if (msgClient != m_msgClient)
        {
            return;
        }
    }

    {{{
        RTP_MSG_USER myUser;
        msgClient->GetUser(&myUser);

        printf(
            "\n"
            " CMsgClient::OnCloseMsg(id : %u-" PRO_PRT64U "-%u,"
            " errorCode : [%d, %d], tcpConnected : %d, server : %s:%u) \n"
            ,
            (unsigned int)myUser.classId,
            myUser.UserId(),
            (unsigned int)myUser.instId,
            (int)errorCode,
            (int)sslCode,
            (int)tcpConnected,
            m_msgConfigInfo.msgc_server_ip.c_str(),
            (unsigned int)m_msgConfigInfo.msgc_server_port
            );
    }}}
}
