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

#include "msg_client.h"
#include "pro/rtp_base.h"
#include "pro/rtp_msg.h"
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
        jmethodID onCloseMsg
        );

    virtual ~CMsgClientJni();

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

private:

    const jobject   m_listener;
    const jmethodID m_onOkMsg;
    const jmethodID m_onRecvMsg;
    const jmethodID m_onCloseMsg;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif /* MSG_CLIENT_JNI_H */
