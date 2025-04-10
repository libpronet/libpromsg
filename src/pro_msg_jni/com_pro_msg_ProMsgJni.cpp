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

#include "com_pro_msg_ProMsgJni.h"
#include "jni_util.h"
#include "msg_client_jni.h"
#include "msg_server_jni.h"
#include "pronet/pro_memory_pool.h"
#include "pronet/pro_thread.h"
#include "pronet/pro_thread_mutex.h"
#include "pronet/pro_time_util.h"
#include "pronet/pro_version.h"
#include "pronet/pro_z.h"
#include <jni.h>

#if defined(__cplusplus)
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
////

struct JAVA_USER_META
{
    JAVA_USER_META()
    {
        clazz       = NULL;
        mid_ctor0   = NULL;
        mid_ctor3   = NULL;
        fid_classId = NULL;
        fid_userId  = NULL;
        fid_instId  = NULL;
    }

    jclass    clazz;
    jmethodID mid_ctor0;
    jmethodID mid_ctor3;
    jfieldID  fid_classId;
    jfieldID  fid_userId;
    jfieldID  fid_instId;

    DECLARE_SGI_POOL(0)
};

static IProReactor*      g_s_reactor = NULL;
static CProStlSet<jlong> g_s_clients;
static CProStlSet<jlong> g_s_servers;
static JAVA_USER_META    g_s_meta;
static CProThreadMutex   g_s_lock;

/////////////////////////////////////////////////////////////////////////////
////

/* static */
jstring
NewJavaString_i(JNIEnv*     env,
                const char* utf8String)
{
    if (env == NULL || utf8String == NULL)
    {
        return NULL;
    }

    jstring javaString = env->NewStringUTF(utf8String);
    if (env->ExceptionCheck())
    {
        javaString = NULL;
    }

    return javaString;
}

/* static */
jobject
NewJavaUser_i(JNIEnv*             env,
              const RTP_MSG_USER& user)
{
    if (env == NULL)
    {
        return NULL;
    }

    jobject javaUser = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_meta.clazz == NULL)
        {
            return NULL;
        }

        javaUser = env->NewObject(
            g_s_meta.clazz,
            g_s_meta.mid_ctor3,
            (jshort)user.classId,
            (jlong) user.UserId(),
            (jint)  user.instId
            );
        if (env->ExceptionCheck())
        {
            javaUser = NULL;
        }
    }

    return javaUser;
}

static
void
MSG_USER_java2cpp_i(JNIEnv*       env,
                    jobject       javaUser,
                    RTP_MSG_USER& cppUser)
{
    cppUser.Zero();

    if (env == NULL || javaUser == NULL)
    {
        return;
    }

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_meta.clazz == NULL)
        {
            return;
        }

        jshort classId = env->GetShortField(javaUser, g_s_meta.fid_classId);
        jlong  userId  = env->GetLongField (javaUser, g_s_meta.fid_userId);
        jint   instId  = env->GetIntField  (javaUser, g_s_meta.fid_instId);
        if (classId <= 0 || classId > 255 || userId < 0 || instId < 0 || instId > 65535)
        {
            return;
        }

        cppUser.classId = (unsigned char)classId;
        cppUser.UserId((uint64_t)userId);
        cppUser.instId  = (uint16_t)instId;
    }
}

static
void
MSG_USER_cpp2java_i(JNIEnv*             env,
                    const RTP_MSG_USER& cppUser,
                    jobject             javaUser)
{
    if (env == NULL || javaUser == NULL)
    {
        return;
    }

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_meta.clazz == NULL)
        {
            return;
        }

        env->SetShortField(javaUser, g_s_meta.fid_classId, (jshort)cppUser.classId);
        env->SetLongField (javaUser, g_s_meta.fid_userId , (jlong) cppUser.UserId());
        env->SetIntField  (javaUser, g_s_meta.fid_instId , (jint)  cppUser.instId);
    }
}

/////////////////////////////////////////////////////////////////////////////
////

JNIEXPORT
jint
JNICALL
JNI_OnLoad(JavaVM* jvm,
           void*   reserved)
{
    JniUtilOnLoad(jvm, JNI_VERSION_1_6);

    return JNI_VERSION_1_6;
}

JNIEXPORT
void
JNICALL
Java_com_pro_msg_ProMsgJni_getCoreVersion(JNIEnv*     env,
                                          jclass      clazz,
                                          jshortArray major_1,
                                          jshortArray minor_1,
                                          jshortArray patch_1)
{
    assert(major_1 != NULL);
    assert(minor_1 != NULL);
    assert(patch_1 != NULL);
    if (major_1 == NULL || minor_1 == NULL || patch_1 == NULL)
    {
        return;
    }

    if (env->GetArrayLength(major_1) <= 0 ||
        env->GetArrayLength(minor_1) <= 0 ||
        env->GetArrayLength(patch_1) <= 0)
    {
        return;
    }

    {
        jshort* p = env->GetShortArrayElements(major_1, NULL);
        if (p == NULL || env->ExceptionCheck())
        {
            return;
        }

        *p = PRO_VER_MAJOR;
        env->ReleaseShortArrayElements(major_1, p, 0);
    }

    {
        jshort* p = env->GetShortArrayElements(minor_1, NULL);
        if (p == NULL || env->ExceptionCheck())
        {
            return;
        }

        *p = PRO_VER_MINOR;
        env->ReleaseShortArrayElements(minor_1, p, 0);
    }

    {
        jshort* p = env->GetShortArrayElements(patch_1, NULL);
        if (p == NULL || env->ExceptionCheck())
        {
            return;
        }

        *p = PRO_VER_PATCH;
        env->ReleaseShortArrayElements(patch_1, p, 0);
    }
}

JNIEXPORT
jboolean
JNICALL
Java_com_pro_msg_ProMsgJni_init(JNIEnv* env,
                                jclass  clazz,
                                jint    threadCount) /* 1 ~ (2/20) ~ 100 */
{
    assert(threadCount > 0);
    assert(threadCount <= 100);
    if (threadCount <= 0 || threadCount > 100)
    {
        return JNI_FALSE;
    }

    IProReactor*   reactor = NULL;
    JAVA_USER_META meta;

    {
        CProThreadMutexGuard mon(g_s_lock);

        assert(g_s_reactor == NULL);
        if (g_s_reactor != NULL)
        {
            return JNI_FALSE;
        }

        reactor = ProCreateReactor((unsigned int)threadCount);
        if (reactor == NULL)
        {
            goto EXIT;
        }

        jclass clazz = env->FindClass("com/pro/msg/ProMsgJni$PRO_MSG_USER");
        if (clazz == NULL || env->ExceptionCheck())
        {
            goto EXIT;
        }

        meta.clazz = (jclass)env->NewGlobalRef(clazz);
        if (meta.clazz == NULL || env->ExceptionCheck())
        {
            goto EXIT;
        }

        meta.mid_ctor0 = env->GetMethodID(meta.clazz, "<init>", "()V");
        if (meta.mid_ctor0 == NULL || env->ExceptionCheck())
        {
            goto EXIT;
        }

        meta.mid_ctor3 = env->GetMethodID(meta.clazz, "<init>", "(SJI)V");
        if (meta.mid_ctor3 == NULL || env->ExceptionCheck())
        {
            goto EXIT;
        }

        meta.fid_classId = env->GetFieldID(meta.clazz, "classId", "S");
        if (meta.fid_classId == NULL || env->ExceptionCheck())
        {
            goto EXIT;
        }

        meta.fid_userId = env->GetFieldID(meta.clazz, "userId", "J");
        if (meta.fid_userId == NULL || env->ExceptionCheck())
        {
            goto EXIT;
        }

        meta.fid_instId = env->GetFieldID(meta.clazz, "instId", "I");
        if (meta.fid_instId == NULL || env->ExceptionCheck())
        {
            goto EXIT;
        }

        g_s_reactor = reactor;
        g_s_meta    = meta;
    }

    {{{
        CProStlString timeString;
        ProGetLocalTimeString(timeString);

        printf(
            "\n"
            "%s \n"
            " Java_com_pro_msg_ProMsgJni_init(threadCount : %u, processId : %u/0x%X) \n"
            ,
            timeString.c_str(),
            (unsigned int)threadCount,
            (unsigned int)ProGetProcessId(),
            (unsigned int)ProGetProcessId()
            );
    }}}

    return JNI_TRUE;

EXIT:

    if (meta.clazz != NULL)
    {
        env->DeleteGlobalRef(meta.clazz);
    }

    ProDeleteReactor(reactor);

    return JNI_FALSE;
}

JNIEXPORT
void
JNICALL
Java_com_pro_msg_ProMsgJni_fini(JNIEnv* env,
                                jclass  clazz)
{
    IProReactor*      reactor = NULL;
    CProStlSet<jlong> clients;
    CProStlSet<jlong> servers;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_reactor == NULL)
        {
            return;
        }

        env->DeleteGlobalRef(g_s_meta.clazz);
        g_s_meta.clazz = NULL;

        servers = g_s_servers;
        g_s_servers.clear();
        clients = g_s_clients;
        g_s_clients.clear();
        reactor = g_s_reactor;
        g_s_reactor = NULL;
    }

    auto itr = servers.begin();
    auto end = servers.end();

    for (; itr != end; ++itr)
    {
        CMsgServerJni* p = (CMsgServerJni*)*itr;
        p->Fini();
        p->Release();
    }

    itr = clients.begin();
    end = clients.end();

    for (; itr != end; ++itr)
    {
        CMsgClientJni* p = (CMsgClientJni*)*itr;
        p->Fini();
        p->Release();
    }

    ProDeleteReactor(reactor);
}

/*-------------------------------------------------------------------------*/

JNIEXPORT
jlong
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientCreate(JNIEnv* env,
                                           jclass  clazz,
                                           jobject listener,
                                           jstring configFileName,
                                           jshort  mmType,     /* = 0, 11 ~ 20 */
                                           jstring serverIp,   /* = null */
                                           jint    serverPort, /* = 0, 1 ~ 65535 */
                                           jobject user,       /* = null */
                                           jstring password,   /* = null */
                                           jstring localIp)    /* = null */
{
    assert(listener != NULL);
    assert(configFileName != NULL);
    if (listener == NULL || configFileName == NULL)
    {
        return 0;
    }

    char           cppConfigFileName[1024] = "";
    RTP_MM_TYPE    cppMmType               = 0;
    char           cppServerIp[64]         = "";
    unsigned short cppServerPort           = 0;
    RTP_MSG_USER   cppUser;
    char           cppPassword[64]         = "";
    char           cppLocalIp[64]          = "";

    cppConfigFileName[sizeof(cppConfigFileName) - 1] = '\0';
    cppServerIp[sizeof(cppServerIp) - 1]             = '\0';
    cppPassword[sizeof(cppPassword) - 1]             = '\0';
    cppLocalIp[sizeof(cppLocalIp) - 1]               = '\0';

    {
        jsize uniSize = env->GetStringLength(configFileName);
        jsize utfSize = env->GetStringUTFLength(configFileName);
        if (utfSize > 0 && utfSize < (jsize)sizeof(cppConfigFileName))
        {
            env->GetStringUTFRegion(configFileName, 0, uniSize, cppConfigFileName);
        }

        if (env->ExceptionCheck())
        {
            return 0;
        }
    }

    if (mmType >= (jshort)RTP_MMT_MSG_MIN && mmType <= (jshort)RTP_MMT_MSG_MAX)
    {
        cppMmType = (RTP_MM_TYPE)mmType;
    }

    if (serverIp != NULL)
    {
        jsize uniSize = env->GetStringLength(serverIp);
        jsize utfSize = env->GetStringUTFLength(serverIp);
        if (utfSize > 0 && utfSize < (jsize)sizeof(cppServerIp))
        {
            env->GetStringUTFRegion(serverIp, 0, uniSize, cppServerIp);
        }

        if (env->ExceptionCheck())
        {
            return 0;
        }
    }

    if (serverPort > 0 && serverPort <= 65535)
    {
        cppServerPort = (unsigned short)serverPort;
    }

    if (user != NULL)
    {
        MSG_USER_java2cpp_i(env, user, cppUser);
    }

    if (password != NULL)
    {
        jsize uniSize = env->GetStringLength(password);
        jsize utfSize = env->GetStringUTFLength(password);
        if (utfSize > 0 && utfSize < (jsize)sizeof(cppPassword))
        {
            env->GetStringUTFRegion(password, 0, uniSize, cppPassword);
        }

        if (env->ExceptionCheck())
        {
            return 0;
        }
    }

    if (localIp != NULL)
    {
        jsize uniSize = env->GetStringLength(localIp);
        jsize utfSize = env->GetStringUTFLength(localIp);
        if (utfSize > 0 && utfSize < (jsize)sizeof(cppLocalIp))
        {
            env->GetStringUTFRegion(localIp, 0, uniSize, cppLocalIp);
        }

        if (env->ExceptionCheck())
        {
            return 0;
        }
    }

    CMsgClientJni* client = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        assert(g_s_reactor != NULL);
        if (g_s_reactor == NULL)
        {
            return 0;
        }

        client = CMsgClientJni::CreateInstance(env, listener);
        if (client == NULL)
        {
            return 0;
        }

        if (!client->Init(g_s_reactor, NULL, cppConfigFileName, cppMmType,
            cppServerIp, cppServerPort, &cppUser, cppPassword, cppLocalIp))
        {
            client->Release();

            return 0;
        }

        g_s_clients.insert((jlong)client);
    }

    return (jlong)client;
}

JNIEXPORT
void
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientDelete(JNIEnv* env,
                                           jclass  clazz,
                                           jlong   client)
{
    if (client == 0)
    {
        return;
    }

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_reactor == NULL)
        {
            return;
        }

        if (g_s_clients.find(client) == g_s_clients.end())
        {
            return;
        }

        g_s_clients.erase(client);
    }

    CMsgClientJni* p = (CMsgClientJni*)client;
    p->Fini();
    p->Release();
}

JNIEXPORT
jshort
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientGetMmType(JNIEnv* evn,
                                              jclass  clazz,
                                              jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return 0;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_clients.find(client) != g_s_clients.end())
        {
            client2 = (CMsgClientJni*)client;
            client2->AddRef();
        }
    }

    jshort mmType = 0;

    if (client2 != NULL)
    {
        mmType = client2->GetMmType();
        client2->Release();
    }

    return mmType;
}

JNIEXPORT
jobject
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientGetUser(JNIEnv* env,
                                            jclass  clazz,
                                            jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return NULL;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_clients.find(client) != g_s_clients.end())
        {
            client2 = (CMsgClientJni*)client;
            client2->AddRef();
        }
    }

    jobject javaUser = NULL;

    if (client2 != NULL)
    {
        RTP_MSG_USER cppUser;
        client2->GetUser(cppUser);

        javaUser = NewJavaUser_i(env, cppUser);
        client2->Release();
    }

    return javaUser;
}

JNIEXPORT
jstring
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientGetSslSuite(JNIEnv* env,
                                                jclass  clazz,
                                                jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return NULL;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_clients.find(client) != g_s_clients.end())
        {
            client2 = (CMsgClientJni*)client;
            client2->AddRef();
        }
    }

    jstring javaSuiteName = NULL;

    if (client2 != NULL)
    {
        char cppSuiteName[64] = "";
        client2->GetSslSuite(cppSuiteName);

        javaSuiteName = NewJavaString_i(env, cppSuiteName);
        client2->Release();
    }

    return javaSuiteName;
}

JNIEXPORT
jstring
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientGetLocalIp(JNIEnv* env,
                                               jclass  clazz,
                                               jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return NULL;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_clients.find(client) != g_s_clients.end())
        {
            client2 = (CMsgClientJni*)client;
            client2->AddRef();
        }
    }

    jstring javaLocalIp = NULL;

    if (client2 != NULL)
    {
        char cppLocalIp[64] = "";
        client2->GetLocalIp(cppLocalIp);

        javaLocalIp = NewJavaString_i(env, cppLocalIp);
        client2->Release();
    }

    return javaLocalIp;
}

JNIEXPORT
jint
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientGetLocalPort(JNIEnv* env,
                                                 jclass  clazz,
                                                 jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return 0;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_clients.find(client) != g_s_clients.end())
        {
            client2 = (CMsgClientJni*)client;
            client2->AddRef();
        }
    }

    jint localPort = 0;

    if (client2 != NULL)
    {
        localPort = client2->GetLocalPort();
        client2->Release();
    }

    return localPort;
}

JNIEXPORT
jstring
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientGetRemoteIp(JNIEnv* env,
                                                jclass  clazz,
                                                jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return NULL;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_clients.find(client) != g_s_clients.end())
        {
            client2 = (CMsgClientJni*)client;
            client2->AddRef();
        }
    }

    jstring javaRemoteIp = NULL;

    if (client2 != NULL)
    {
        char cppRemoteIp[64] = "";
        client2->GetRemoteIp(cppRemoteIp);

        javaRemoteIp = NewJavaString_i(env, cppRemoteIp);
        client2->Release();
    }

    return javaRemoteIp;
}

JNIEXPORT
jint
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientGetRemotePort(JNIEnv* env,
                                                  jclass  clazz,
                                                  jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return 0;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_clients.find(client) != g_s_clients.end())
        {
            client2 = (CMsgClientJni*)client;
            client2->AddRef();
        }
    }

    jint remotePort = 0;

    if (client2 != NULL)
    {
        remotePort = client2->GetRemotePort();
        client2->Release();
    }

    return remotePort;
}

JNIEXPORT
jboolean
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientSendMsg(JNIEnv*      env,
                                            jclass       clazz,
                                            jlong        client,
                                            jbyteArray   buf,
                                            jint         charset,  /* 0 ~ 65535 */
                                            jobjectArray dstUsers) /* count <= 255 */
{
    jboolean ret = Java_com_pro_msg_ProMsgJni_msgClientSendMsg2(
        env, clazz, client, buf, NULL, charset, dstUsers);

    return ret;
}

JNIEXPORT
jboolean
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientSendMsg2(JNIEnv*      env,
                                             jclass       clazz,
                                             jlong        client,
                                             jbyteArray   buf1,
                                             jbyteArray   buf2,     /* = null */
                                             jint         charset,  /* 0 ~ 65535 */
                                             jobjectArray dstUsers) /* count <= 255 */
{
    assert(client != 0);
    if (client == 0 || buf1 == NULL || charset < 0 || charset > 65535 || dstUsers == NULL)
    {
        return JNI_FALSE;
    }

    CProStlVector<RTP_MSG_USER> cppDstUsers;

    {
        int i = 0;
        int c = (int)env->GetArrayLength(dstUsers);

        if (c <= 0 || c > 255)
        {
            return JNI_FALSE;
        }

        for (; i < c; ++i)
        {
            jobject javaUser = env->GetObjectArrayElement(dstUsers, i);
            if (javaUser == NULL || env->ExceptionCheck())
            {
                return JNI_FALSE;
            }

            RTP_MSG_USER cppUser;
            MSG_USER_java2cpp_i(env, javaUser, cppUser);

            cppDstUsers.push_back(cppUser);
            env->DeleteLocalRef(javaUser);
        }
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_reactor == NULL)
        {
            return JNI_FALSE;
        }

        if (g_s_clients.find(client) == g_s_clients.end())
        {
            return JNI_FALSE;
        }

        client2 = (CMsgClientJni*)client;
        client2->AddRef();
    }

    jsize  buf1_size = env->GetArrayLength(buf1);
    jbyte* buf1_p    = env->GetByteArrayElements(buf1, NULL);
    if (buf1_size <= 0 || buf1_p == NULL || env->ExceptionCheck())
    {
        client2->Release();

        return JNI_FALSE;
    }

    jsize  buf2_size = 0;
    jbyte* buf2_p    = NULL;
    if (buf2 != NULL)
    {
        buf2_size = env->GetArrayLength(buf2);
        buf2_p    = env->GetByteArrayElements(buf2, NULL);
        if (buf2_size <= 0 || buf2_p == NULL || env->ExceptionCheck())
        {
            env->ReleaseByteArrayElements(buf1, buf1_p, JNI_ABORT);
            client2->Release();

            return JNI_FALSE;
        }
    }

    bool ret = client2->SendMsg2(
        buf1_p,
        buf1_size,
        buf2_p,
        buf2_size,
        (uint16_t)charset,
        &cppDstUsers[0],
        (unsigned char)cppDstUsers.size()
        );
    env->ReleaseByteArrayElements(buf1, buf1_p, JNI_ABORT);
    if (buf2 != NULL)
    {
        env->ReleaseByteArrayElements(buf2, buf2_p, JNI_ABORT);
    }
    client2->Release();

    return ret ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT
void
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientSetOutputRedline(JNIEnv* env,
                                                     jclass  clazz,
                                                     jlong   client,
                                                     jlong   redlineBytes)
{
    assert(client != 0);
    if (client == 0 || redlineBytes <= 0)
    {
        return;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_reactor == NULL)
        {
            return;
        }

        if (g_s_clients.find(client) == g_s_clients.end())
        {
            return;
        }

        client2 = (CMsgClientJni*)client;
        client2->AddRef();
    }

    client2->SetOutputRedline((size_t)redlineBytes);
    client2->Release();
}

JNIEXPORT
jlong
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientGetOutputRedline(JNIEnv* env,
                                                     jclass  clazz,
                                                     jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return 0;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_clients.find(client) != g_s_clients.end())
        {
            client2 = (CMsgClientJni*)client;
            client2->AddRef();
        }
    }

    jlong redlineBytes = 0;

    if (client2 != NULL)
    {
        redlineBytes = client2->GetOutputRedline();
        client2->Release();
    }

    return redlineBytes;
}

JNIEXPORT
jlong
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientGetSendingBytes(JNIEnv* env,
                                                    jclass  clazz,
                                                    jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return 0;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_clients.find(client) != g_s_clients.end())
        {
            client2 = (CMsgClientJni*)client;
            client2->AddRef();
        }
    }

    jlong sendingBytes = 0;

    if (client2 != NULL)
    {
        sendingBytes = client2->GetSendingBytes();
        client2->Release();
    }

    return sendingBytes;
}

JNIEXPORT
jboolean
JNICALL
Java_com_pro_msg_ProMsgJni_msgClientReconnect(JNIEnv* env,
                                              jclass  clazz,
                                              jlong   client)
{
    assert(client != 0);
    if (client == 0)
    {
        return JNI_FALSE;
    }

    CMsgClientJni* client2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_reactor == NULL)
        {
            return JNI_FALSE;
        }

        if (g_s_clients.find(client) == g_s_clients.end())
        {
            return JNI_FALSE;
        }

        client2 = (CMsgClientJni*)client;
        client2->AddRef();
    }

    bool ret = client2->Reconnect();
    client2->Release();

    return ret ? JNI_TRUE : JNI_FALSE;
}

/*-------------------------------------------------------------------------*/

JNIEXPORT
jlong
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerCreate(JNIEnv* env,
                                           jclass  clazz,
                                           jobject listener,
                                           jstring configFileName,
                                           jshort  mmType,         /* = 0, 11 ~ 20 */
                                           jint    serviceHubPort) /* = 0, 1 ~ 65535 */
{
    assert(listener != NULL);
    assert(configFileName != NULL);
    if (listener == NULL || configFileName == NULL)
    {
        return 0;
    }

    char           cppConfigFileName[1024] = "";
    RTP_MM_TYPE    cppMmType               = 0;
    unsigned short cppServiceHubPort       = 0;

    cppConfigFileName[sizeof(cppConfigFileName) - 1] = '\0';

    {
        jsize uniSize = env->GetStringLength(configFileName);
        jsize utfSize = env->GetStringUTFLength(configFileName);
        if (utfSize > 0 && utfSize < (jsize)sizeof(cppConfigFileName))
        {
            env->GetStringUTFRegion(configFileName, 0, uniSize, cppConfigFileName);
        }

        if (env->ExceptionCheck())
        {
            return 0;
        }
    }

    if (mmType >= (jshort)RTP_MMT_MSG_MIN && mmType <= (jshort)RTP_MMT_MSG_MAX)
    {
        cppMmType = (RTP_MM_TYPE)mmType;
    }

    if (serviceHubPort > 0 && serviceHubPort <= 65535)
    {
        cppServiceHubPort = (unsigned short)serviceHubPort;
    }

    CMsgServerJni* server = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        assert(g_s_reactor != NULL);
        if (g_s_reactor == NULL)
        {
            return 0;
        }

        server = CMsgServerJni::CreateInstance(env, listener);
        if (server == NULL)
        {
            return 0;
        }

        if (!server->Init(g_s_reactor, NULL, cppConfigFileName, cppMmType, cppServiceHubPort))
        {
            server->Release();

            return 0;
        }

        g_s_servers.insert((jlong)server);
    }

    return (jlong)server;
}

JNIEXPORT
void
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerDelete(JNIEnv* env,
                                           jclass  clazz,
                                           jlong   server)
{
    if (server == 0)
    {
        return;
    }

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_reactor == NULL)
        {
            return;
        }

        if (g_s_servers.find(server) == g_s_servers.end())
        {
            return;
        }

        g_s_servers.erase(server);
    }

    CMsgServerJni* p = (CMsgServerJni*)server;
    p->Fini();
    p->Release();
}

JNIEXPORT
jshort
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerGetMmType(JNIEnv* evn,
                                              jclass  clazz,
                                              jlong   server)
{
    assert(server != 0);
    if (server == 0)
    {
        return 0;
    }

    CMsgServerJni* server2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_servers.find(server) != g_s_servers.end())
        {
            server2 = (CMsgServerJni*)server;
            server2->AddRef();
        }
    }

    jshort mmType = 0;

    if (server2 != NULL)
    {
        mmType = server2->GetMmType();
        server2->Release();
    }

    return mmType;
}

JNIEXPORT
jint
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerGetServicePort(JNIEnv* env,
                                                   jclass  clazz,
                                                   jlong   server)
{
    assert(server != 0);
    if (server == 0)
    {
        return 0;
    }

    CMsgServerJni* server2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_servers.find(server) != g_s_servers.end())
        {
            server2 = (CMsgServerJni*)server;
            server2->AddRef();
        }
    }

    jint servicePort = 0;

    if (server2 != NULL)
    {
        servicePort = server2->GetServicePort();
        server2->Release();
    }

    return servicePort;
}

JNIEXPORT
jstring
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerGetSslSuite(JNIEnv* env,
                                                jclass  clazz,
                                                jlong   server,
                                                jobject user)
{
    assert(server != 0);
    assert(user != NULL);
    if (server == 0 || user == NULL)
    {
        return NULL;
    }

    RTP_MSG_USER cppUser;
    MSG_USER_java2cpp_i(env, user, cppUser);

    CMsgServerJni* server2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_servers.find(server) != g_s_servers.end())
        {
            server2 = (CMsgServerJni*)server;
            server2->AddRef();
        }
    }

    jstring javaSuiteName = NULL;

    if (server2 != NULL)
    {
        char cppSuiteName[64] = "";
        server2->GetSslSuite(cppUser, cppSuiteName);

        javaSuiteName = NewJavaString_i(env, cppSuiteName);
        server2->Release();
    }

    return javaSuiteName;
}

JNIEXPORT
jlong
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerGetUserCount(JNIEnv* env,
                                                 jclass  clazz,
                                                 jlong   server)
{
    assert(server != 0);
    if (server == 0)
    {
        return 0;
    }

    CMsgServerJni* server2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_servers.find(server) != g_s_servers.end())
        {
            server2 = (CMsgServerJni*)server;
            server2->AddRef();
        }
    }

    jlong userCount = 0;

    if (server2 != NULL)
    {
        userCount = server2->GetUserCount();
        server2->Release();
    }

    return userCount;
}

JNIEXPORT
void
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerKickoutUser(JNIEnv* env,
                                                jclass  clazz,
                                                jlong   server,
                                                jobject user)
{
    assert(server != 0);
    if (server == 0 || user == NULL)
    {
        return;
    }

    RTP_MSG_USER cppUser;
    MSG_USER_java2cpp_i(env, user, cppUser);

    CMsgServerJni* server2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_reactor == NULL)
        {
            return;
        }

        if (g_s_servers.find(server) == g_s_servers.end())
        {
            return;
        }

        server2 = (CMsgServerJni*)server;
        server2->AddRef();
    }

    server2->KickoutUser(cppUser);
    server2->Release();
}

JNIEXPORT
jboolean
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerSendMsg(JNIEnv*      env,
                                            jclass       clazz,
                                            jlong        server,
                                            jbyteArray   buf,
                                            jint         charset,  /* 0 ~ 65535 */
                                            jobjectArray dstUsers) /* count <= 255 */
{
    jboolean ret = Java_com_pro_msg_ProMsgJni_msgServerSendMsg2(
        env, clazz, server, buf, NULL, charset, dstUsers);

    return ret;
}

JNIEXPORT
jboolean
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerSendMsg2(JNIEnv*      env,
                                             jclass       clazz,
                                             jlong        server,
                                             jbyteArray   buf1,
                                             jbyteArray   buf2,     /* = null */
                                             jint         charset,  /* 0 ~ 65535 */
                                             jobjectArray dstUsers) /* count <= 255 */
{
    assert(server != 0);
    if (server == 0 || buf1 == NULL || charset < 0 || charset > 65535 || dstUsers == NULL)
    {
        return JNI_FALSE;
    }

    CProStlVector<RTP_MSG_USER> cppDstUsers;

    {
        int i = 0;
        int c = (int)env->GetArrayLength(dstUsers);

        if (c <= 0 || c > 255)
        {
            return JNI_FALSE;
        }

        for (; i < c; ++i)
        {
            jobject javaUser = env->GetObjectArrayElement(dstUsers, i);
            if (javaUser == NULL || env->ExceptionCheck())
            {
                return JNI_FALSE;
            }

            RTP_MSG_USER cppUser;
            MSG_USER_java2cpp_i(env, javaUser, cppUser);

            cppDstUsers.push_back(cppUser);
            env->DeleteLocalRef(javaUser);
        }
    }

    CMsgServerJni* server2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_reactor == NULL)
        {
            return JNI_FALSE;
        }

        if (g_s_servers.find(server) == g_s_servers.end())
        {
            return JNI_FALSE;
        }

        server2 = (CMsgServerJni*)server;
        server2->AddRef();
    }

    jsize  buf1_size = env->GetArrayLength(buf1);
    jbyte* buf1_p    = env->GetByteArrayElements(buf1, NULL);
    if (buf1_size <= 0 || buf1_p == NULL || env->ExceptionCheck())
    {
        server2->Release();

        return JNI_FALSE;
    }

    jsize  buf2_size = 0;
    jbyte* buf2_p    = NULL;
    if (buf2 != NULL)
    {
        buf2_size = env->GetArrayLength(buf2);
        buf2_p    = env->GetByteArrayElements(buf2, NULL);
        if (buf2_size <= 0 || buf2_p == NULL || env->ExceptionCheck())
        {
            env->ReleaseByteArrayElements(buf1, buf1_p, JNI_ABORT);
            server2->Release();

            return JNI_FALSE;
        }
    }

    bool ret = server2->SendMsg2(
        buf1_p,
        buf1_size,
        buf2_p,
        buf2_size,
        (uint16_t)charset,
        &cppDstUsers[0],
        (unsigned char)cppDstUsers.size()
        );
    env->ReleaseByteArrayElements(buf1, buf1_p, JNI_ABORT);
    if (buf2 != NULL)
    {
        env->ReleaseByteArrayElements(buf2, buf2_p, JNI_ABORT);
    }
    server2->Release();

    return ret ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT
void
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerSetOutputRedline(JNIEnv* env,
                                                     jclass  clazz,
                                                     jlong   server,
                                                     jlong   redlineBytes)
{
    assert(server != 0);
    if (server == 0 || redlineBytes <= 0)
    {
        return;
    }

    CMsgServerJni* server2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_reactor == NULL)
        {
            return;
        }

        if (g_s_servers.find(server) == g_s_servers.end())
        {
            return;
        }

        server2 = (CMsgServerJni*)server;
        server2->AddRef();
    }

    server2->SetOutputRedline((size_t)redlineBytes);
    server2->Release();
}

JNIEXPORT
jlong
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerGetOutputRedline(JNIEnv* env,
                                                     jclass  clazz,
                                                     jlong   server)
{
    assert(server != 0);
    if (server == 0)
    {
        return 0;
    }

    CMsgServerJni* server2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_servers.find(server) != g_s_servers.end())
        {
            server2 = (CMsgServerJni*)server;
            server2->AddRef();
        }
    }

    jlong redlineBytes = 0;

    if (server2 != NULL)
    {
        redlineBytes = server2->GetOutputRedline();
        server2->Release();
    }

    return redlineBytes;
}

JNIEXPORT
jlong
JNICALL
Java_com_pro_msg_ProMsgJni_msgServerGetSendingBytes(JNIEnv* env,
                                                    jclass  clazz,
                                                    jlong   server,
                                                    jobject user)
{
    assert(server != 0);
    if (server == 0 || user == NULL)
    {
        return 0;
    }

    RTP_MSG_USER cppUser;
    MSG_USER_java2cpp_i(env, user, cppUser);

    CMsgServerJni* server2 = NULL;

    {
        CProThreadMutexGuard mon(g_s_lock);

        if (g_s_servers.find(server) != g_s_servers.end())
        {
            server2 = (CMsgServerJni*)server;
            server2->AddRef();
        }
    }

    jlong sendingBytes = 0;

    if (server2 != NULL)
    {
        sendingBytes = server2->GetSendingBytes(cppUser);
        server2->Release();
    }

    return sendingBytes;
}

/////////////////////////////////////////////////////////////////////////////
////

#if defined(__cplusplus)
} /* extern "C" */
#endif
