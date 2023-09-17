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

#include "msg_server_jni.h"
#include "jni_util.h"
#include "pronet/pro_memory_pool.h"
#include "pronet/pro_thread_mutex.h"
#include "pronet/pro_z.h"
#include "pronet/rtp_base.h"
#include "pronet/rtp_msg.h"
#include "../pro_msg/msg_server.h"
#include <jni.h>

/////////////////////////////////////////////////////////////////////////////
////

#if defined(__cplusplus)
extern "C" {
#endif

extern
jstring
NewJavaString_i(JNIEnv*     env,
                const char* utf8String);

extern
jobject
NewJavaUser_i(JNIEnv*             env,
              const RTP_MSG_USER& user);

#if defined(__cplusplus)
} /* extern "C" */
#endif

/////////////////////////////////////////////////////////////////////////////
////

CMsgServerJni*
CMsgServerJni::CreateInstance(JNIEnv* env,
                              jobject listener)
{
    assert(env != NULL);
    assert(listener != NULL);
    if (env == NULL || listener == NULL)
    {
        return (NULL);
    }

    const jclass clazz = env->GetObjectClass(listener);
    if (clazz == NULL || env->ExceptionCheck())
    {
        return (NULL);
    }

    jmethodID onOkUser        = NULL;
    jmethodID onCloseUser     = NULL;
    jmethodID onHeartbeatUser = NULL;
    jmethodID onRecvMsg       = NULL;

    onOkUser = env->GetMethodID(clazz, "msgServerOnOkUser",
        "(JLcom/pro/msg/ProMsgJni$PRO_MSG_USER;Ljava/lang/String;)V");
    if (onOkUser == NULL || env->ExceptionCheck())
    {
        return (NULL);
    }

    onCloseUser = env->GetMethodID(clazz, "msgServerOnCloseUser",
        "(JLcom/pro/msg/ProMsgJni$PRO_MSG_USER;II)V");
    if (onCloseUser == NULL || env->ExceptionCheck())
    {
        return (NULL);
    }

    onHeartbeatUser = env->GetMethodID(clazz, "msgServerOnHeartbeatUser",
        "(JLcom/pro/msg/ProMsgJni$PRO_MSG_USER;J)V");
    if (onHeartbeatUser == NULL || env->ExceptionCheck())
    {
        return (NULL);
    }

    onRecvMsg = env->GetMethodID(clazz, "msgServerOnRecvMsg",
        "(J[BILcom/pro/msg/ProMsgJni$PRO_MSG_USER;)V");
    if (onRecvMsg == NULL || env->ExceptionCheck())
    {
        return (NULL);
    }

    const jobject listener2 = env->NewGlobalRef(listener);
    if (listener2 == NULL || env->ExceptionCheck())
    {
        return (NULL);
    }

    CMsgServerJni* const server = new CMsgServerJni(
        listener2, onOkUser, onCloseUser, onHeartbeatUser, onRecvMsg);

    return (server);
}

CMsgServerJni::CMsgServerJni(jobject   listener,
                             jmethodID onOkUser,
                             jmethodID onCloseUser,
                             jmethodID onHeartbeatUser,
                             jmethodID onRecvMsg)
:
m_listener(listener),
m_onOkUser(onOkUser),
m_onCloseUser(onCloseUser),
m_onHeartbeatUser(onHeartbeatUser),
m_onRecvMsg(onRecvMsg)
{
}

CMsgServerJni::~CMsgServerJni()
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
CMsgServerJni::OnOkUser(IRtpMsgServer*      msgServer,
                        const RTP_MSG_USER* user,
                        const char*         userPublicIp,
                        const RTP_MSG_USER* c2sUser, /* = NULL */
                        int64_t             appData)
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
    }

    JNIEnv* const env = JniUtilAttach();
    if (env == NULL)
    {
        return;
    }

    const jobject javaUser = NewJavaUser_i(env, *user);
    if (javaUser == NULL)
    {
        JniUtilDetach();

        return;
    }

    const jstring javaPublicIp = NewJavaString_i(env, userPublicIp);
    if (javaPublicIp == NULL)
    {
        env->DeleteLocalRef(javaUser);
        JniUtilDetach();

        return;
    }

    env->CallVoidMethod(
        m_listener,
        m_onOkUser,
        (jlong)  this,
        (jobject)javaUser,
        (jstring)javaPublicIp
        );
    env->DeleteLocalRef(javaPublicIp);
    env->DeleteLocalRef(javaUser);
    JniUtilDetach();
}

void
CMsgServerJni::OnCloseUser(IRtpMsgServer*      msgServer,
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
    }

    JNIEnv* const env = JniUtilAttach();
    if (env == NULL)
    {
        return;
    }

    const jobject javaUser = NewJavaUser_i(env, *user);
    if (javaUser == NULL)
    {
        JniUtilDetach();

        return;
    }

    env->CallVoidMethod(
        m_listener,
        m_onCloseUser,
        (jlong)  this,
        (jobject)javaUser,
        (jint)   errorCode,
        (jint)   sslCode
        );
    env->DeleteLocalRef(javaUser);
    JniUtilDetach();
}

void
CMsgServerJni::OnHeartbeatUser(IRtpMsgServer*      msgServer,
                               const RTP_MSG_USER* user,
                               int64_t             peerAliveTick)
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
    }

    JNIEnv* const env = JniUtilAttach();
    if (env == NULL)
    {
        return;
    }

    const jobject javaUser = NewJavaUser_i(env, *user);
    if (javaUser == NULL)
    {
        JniUtilDetach();

        return;
    }

    env->CallVoidMethod(
        m_listener,
        m_onHeartbeatUser,
        (jlong)  this,
        (jobject)javaUser,
        (jlong)  peerAliveTick
        );
    env->DeleteLocalRef(javaUser);
    JniUtilDetach();
}

void
CMsgServerJni::OnRecvMsg(IRtpMsgServer*      msgServer,
                         const void*         buf,
                         unsigned long       size,
                         uint16_t            charset,
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
    }

    JNIEnv* const env = JniUtilAttach();
    if (env == NULL)
    {
        return;
    }

    const jbyteArray javaBuf = env->NewByteArray(size);
    if (javaBuf == NULL || env->ExceptionCheck())
    {
        JniUtilDetach();

        return;
    }

    env->SetByteArrayRegion(javaBuf, 0, size, (jbyte*)buf);
    if (env->ExceptionCheck())
    {
        env->DeleteLocalRef(javaBuf);
        JniUtilDetach();

        return;
    }

    const jobject javaUser = NewJavaUser_i(env, *srcUser);
    if (javaUser == NULL)
    {
        env->DeleteLocalRef(javaBuf);
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
    env->DeleteLocalRef(javaUser);
    env->DeleteLocalRef(javaBuf);
    JniUtilDetach();
}
