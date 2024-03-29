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

#if !defined(MSG_SERVER_JNI_H)
#define MSG_SERVER_JNI_H

#include "pronet/pro_memory_pool.h"
#include "pronet/rtp_base.h"
#include "pronet/rtp_msg.h"
#include "../pro_msg/msg_server.h"
#include <jni.h>

/////////////////////////////////////////////////////////////////////////////
////

class CMsgServerJni : public CMsgServer
{
public:

    static CMsgServerJni* CreateInstance(
        JNIEnv* env,
        jobject listener
        );

private:

    CMsgServerJni(
        jobject   listener,
        jmethodID onOkUser,
        jmethodID onCloseUser,
        jmethodID onHeartbeatUser,
        jmethodID onRecvMsg
        );

    virtual ~CMsgServerJni();

    virtual void OnOkUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* user,
        const char*         userPublicIp,
        const RTP_MSG_USER* c2sUser, /* = NULL */
        int64_t             appData
        );

    virtual void OnCloseUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* user,
        int                 errorCode,
        int                 sslCode
        );

    virtual void OnHeartbeatUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* user,
        int64_t             peerAliveTick
        );

    virtual void OnRecvMsg(
        IRtpMsgServer*      msgServer,
        const void*         buf,
        size_t              size,
        uint16_t            charset,
        const RTP_MSG_USER* srcUser
        );

private:

    const jobject   m_listener;
    const jmethodID m_onOkUser;
    const jmethodID m_onCloseUser;
    const jmethodID m_onHeartbeatUser;
    const jmethodID m_onRecvMsg;

    DECLARE_SGI_POOL(0)
};

/////////////////////////////////////////////////////////////////////////////
////

#endif /* MSG_SERVER_JNI_H */
