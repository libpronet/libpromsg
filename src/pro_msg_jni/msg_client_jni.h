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

#if !defined(MSG_CLIENT_JNI_H)
#define MSG_CLIENT_JNI_H

#include "pronet/pro_memory_pool.h"
#include "pronet/rtp_base.h"
#include "pronet/rtp_msg.h"
#include "../pro_msg/msg_client.h"
#include <jni.h>

/////////////////////////////////////////////////////////////////////////////
////

class CMsgClientJni : public CMsgClient
{
public:

    static CMsgClientJni* CreateInstance(
        JNIEnv* env,
        jobject listener
        );

private:

    CMsgClientJni(
        jobject   listener,
        jmethodID onOkMsg,
        jmethodID onRecvMsg,
        jmethodID onCloseMsg,
        jmethodID onHeartbeatMsg
        );

    virtual ~CMsgClientJni();

    virtual void OnOkMsg(
        IRtpMsgClient*      msgClient,
        const RTP_MSG_USER* myUser,
        const char*         myPublicIp
        );

    virtual void OnRecvMsg(
        IRtpMsgClient*      msgClient,
        const void*         buf,
        unsigned long       size,
        PRO_UINT16          charset,
        const RTP_MSG_USER* srcUser
        );

    virtual void OnCloseMsg(
        IRtpMsgClient* msgClient,
        long           errorCode,
        long           sslCode,
        bool           tcpConnected
        );

    virtual void OnHeartbeatMsg(
        IRtpMsgClient* msgClient,
        PRO_INT64      peerAliveTick
        );

private:

    const jobject   m_listener;
    const jmethodID m_onOkMsg;
    const jmethodID m_onRecvMsg;
    const jmethodID m_onCloseMsg;
    const jmethodID m_onHeartbeatMsg;

    DECLARE_SGI_POOL(0)
};

/////////////////////////////////////////////////////////////////////////////
////

#endif /* MSG_CLIENT_JNI_H */
