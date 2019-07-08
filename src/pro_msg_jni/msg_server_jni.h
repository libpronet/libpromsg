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

#include "msg_server.h"
#include "pro/rtp_base.h"
#include "pro/rtp_msg.h"
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
        jmethodID onRecvMsg
        );

    virtual ~CMsgServerJni();

    virtual void PRO_CALLTYPE OnOkUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* user,
        const char*         userPublicIp,
        const RTP_MSG_USER* c2sUser, /* = NULL */
        PRO_INT64           appData
        );

    virtual void PRO_CALLTYPE OnCloseUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* user,
        long                errorCode,
        long                sslCode
        );

    virtual void PRO_CALLTYPE OnRecvMsg(
        IRtpMsgServer*      msgServer,
        const void*         buf,
        unsigned long       size,
        PRO_UINT16          charset,
        const RTP_MSG_USER* srcUser
        );

private:

    const jobject   m_listener;
    const jmethodID m_onOkUser;
    const jmethodID m_onCloseUser;
    const jmethodID m_onRecvMsg;
};

/////////////////////////////////////////////////////////////////////////////
////

#endif /* MSG_SERVER_JNI_H */
