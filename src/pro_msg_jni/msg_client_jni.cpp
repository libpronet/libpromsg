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

#include "msg_client_jni.h"
#include "jni_util.h"
#include "msg_client.h"
#include "pro/pro_thread_mutex.h"
#include "pro/pro_z.h"
#include "pro/rtp_base.h"
#include "pro/rtp_msg.h"
#include <cassert>
#include <jni.h>

/////////////////////////////////////////////////////////////////////////////
////

#if defined(__cplusplus)
extern "C" {
#endif

extern
jstring
PRO_CALLTYPE
NewJavaString_i(JNIEnv*     env,
                const char* utf8String);

extern
jobject
PRO_CALLTYPE
NewJavaUser_i(JNIEnv*             env,
              const RTP_MSG_USER& user);

#if defined(__cplusplus)
}
#endif

/////////////////////////////////////////////////////////////////////////////
////

CMsgClientJni*
CMsgClientJni::CreateInstance(jobject   listener,
                              jmethodID onOkMsg,
                              jmethodID onRecvMsg,
                              jmethodID onCloseMsg)
{
    CMsgClientJni* const client =
        new CMsgClientJni(listener, onOkMsg, onRecvMsg, onCloseMsg);

    return (client);
}

CMsgClientJni::CMsgClientJni(jobject   listener,
                             jmethodID onOkMsg,
                             jmethodID onRecvMsg,
                             jmethodID onCloseMsg)
                             :
m_listener(listener),
m_onOkMsg(onOkMsg),
m_onRecvMsg(onRecvMsg),
m_onCloseMsg(onCloseMsg)
{
}

CMsgClientJni::~CMsgClientJni()
{
    Fini();

    JNIEnv* const env = JniUtilAttach();
    if (env != NULL)
    {
        env->DeleteGlobalRef(m_listener);
        JniUtilDetach();
    }
}

void
PRO_CALLTYPE
CMsgClientJni::OnOkMsg(IRtpMsgClient*      msgClient,
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

    JNIEnv* const env = JniUtilAttach();
    if (env == NULL)
    {
        return;
    }

    const jobject javaUser = NewJavaUser_i(env, *myUser);
    if (javaUser == NULL)
    {
        JniUtilDetach();

        return;
    }

    const jstring javaPublicIp = NewJavaString_i(env, myPublicIp);
    if (javaPublicIp == NULL)
    {
        env->DeleteLocalRef(javaUser); /* release local reference */
        JniUtilDetach();

        return;
    }

    env->CallVoidMethod(
        m_listener,
        m_onOkMsg,
        (jlong)  this,
        (jobject)javaUser,
        (jstring)javaPublicIp
        );
    env->DeleteLocalRef(javaPublicIp); /* release local reference */
    env->DeleteLocalRef(javaUser);     /* release local reference */
    JniUtilDetach();
}

void
PRO_CALLTYPE
CMsgClientJni::OnRecvMsg(IRtpMsgClient*      msgClient,
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

    JNIEnv* const env = JniUtilAttach();
    if (env == NULL)
    {
        return;
    }

    const jbyteArray javaBuf = env->NewByteArray(size);
    if (javaBuf == NULL)
    {
        JniUtilDetach();

        return;
    }
    else
    {
        env->SetByteArrayRegion(javaBuf, 0, size, (jbyte*)buf);
    }

    const jobject javaUser = NewJavaUser_i(env, *srcUser);
    if (javaUser == NULL)
    {
        env->DeleteLocalRef(javaBuf); /* release local reference */
        JniUtilDetach();

        return;
    }

    env->CallVoidMethod(
        m_listener,
        m_onRecvMsg,
        (jlong)     this,
        (jbyteArray)javaBuf,
        (jint)      charset,
        (jobject)   javaUser
        );
    env->DeleteLocalRef(javaUser); /* release local reference */
    env->DeleteLocalRef(javaBuf);  /* release local reference */
    JniUtilDetach();
}

void
PRO_CALLTYPE
CMsgClientJni::OnCloseMsg(IRtpMsgClient* msgClient,
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

    JNIEnv* const env = JniUtilAttach();
    if (env == NULL)
    {
        return;
    }

    env->CallVoidMethod(
        m_listener,
        m_onCloseMsg,
        (jlong)   this,
        (jint)    errorCode,
        (jint)    sslCode,
        (jboolean)(tcpConnected ? JNI_TRUE : JNI_FALSE)
        );
    JniUtilDetach();
}
