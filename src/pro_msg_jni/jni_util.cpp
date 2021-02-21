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

#include "jni_util.h"
#include "pronet/pro_a.h"
#include "pronet/pro_z.h"

#if defined(_WIN32) || defined(_WIN32_WCE)
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <cassert>
#include <jni.h>

#if defined(__cplusplus)
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
////

static JavaVM*       g_s_jvm = NULL;
static jint          g_s_ver = 0;
#if defined(_WIN32) || defined(_WIN32_WCE)
static unsigned long g_s_key = (unsigned long)-1;
#else
static pthread_key_t g_s_key = (pthread_key_t)-1;
#endif

/////////////////////////////////////////////////////////////////////////////
////

static
void
JniUtilCleanup_i(void*)
{
    if (g_s_jvm == NULL)
    {
        return;
    }

    JNIEnv* env = NULL;
    jint    err;

    err = g_s_jvm->GetEnv((void**)&env, g_s_ver);
    if (err != JNI_OK || env == NULL)
    {
        return;
    }

    g_s_jvm->DetachCurrentThread();
}

/////////////////////////////////////////////////////////////////////////////
////

void
PRO_CALLTYPE
JniUtilOnLoad(JavaVM* jvm,
              jint    jdkVer)
{
    assert(jvm != NULL);
    assert(g_s_jvm == NULL);
    if (jvm == NULL || g_s_jvm != NULL)
    {
        return;
    }

    g_s_jvm = jvm;
    g_s_ver = jdkVer;

#if defined(_WIN32) || defined(_WIN32_WCE)
    g_s_key = ::TlsAlloc();
#else
    pthread_key_create(&g_s_key, &JniUtilCleanup_i);
#endif
}

JNIEnv*
PRO_CALLTYPE
JniUtilAttach()
{
    if (g_s_jvm == NULL)
    {
        return (NULL);
    }

    JNIEnv* env = NULL;
    jint    err;

    err = g_s_jvm->GetEnv((void**)&env, g_s_ver);
    if (err == JNI_OK && env != NULL)
    {
        return (env);
    }

#if defined(_WIN32) || defined(_WIN32_WCE)
    env = (JNIEnv*)::TlsGetValue(g_s_key);
#else
    env = (JNIEnv*)pthread_getspecific(g_s_key);
#endif
    if (env != NULL)
    {
        return (env);
    }

#if defined(ANDROID)
    err = g_s_jvm->AttachCurrentThread(&env, NULL);
#else
    err = g_s_jvm->AttachCurrentThread((void**)&env, NULL);
#endif
    if (err != JNI_OK || env == NULL)
    {
        return (NULL);
    }

#if defined(_WIN32) || defined(_WIN32_WCE)
    ::TlsSetValue(g_s_key, env);
#else
    pthread_setspecific(g_s_key, env);
#endif

    return (env);
}

void
PRO_CALLTYPE
JniUtilDetach()
{
    if (g_s_jvm == NULL)
    {
        return;
    }

    JNIEnv* env = NULL;

#if defined(_WIN32) || defined(_WIN32_WCE)
    env = (JNIEnv*)::TlsGetValue(g_s_key);
#else
    env = (JNIEnv*)pthread_getspecific(g_s_key);
#endif
    if (env == NULL)
    {
        return;
    }

    g_s_jvm->DetachCurrentThread();

#if defined(_WIN32) || defined(_WIN32_WCE)
    ::TlsSetValue(g_s_key, NULL);
#else
    pthread_setspecific(g_s_key, NULL);
#endif
}

/////////////////////////////////////////////////////////////////////////////
////

#if defined(__cplusplus)
} /* extern "C" */
#endif
