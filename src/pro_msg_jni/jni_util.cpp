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
#include "pro/pro_a.h"
#include "pro/pro_z.h"

#if !defined(WIN32) && !defined(_WIN32_WCE)
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
#if !defined(WIN32) && !defined(_WIN32_WCE)
static pthread_key_t g_s_key = (pthread_key_t)-1;
#endif

/////////////////////////////////////////////////////////////////////////////
////

static
void
JniUtilCleanup_i(void* env)
{
    if (env == NULL || g_s_jvm == NULL)
    {
        return;
    }

#if !defined(WIN32) && !defined(_WIN32_WCE)
    g_s_jvm->DetachCurrentThread();
#endif
}

static
void
PRO_CALLTYPE
JniUtilStartup_i(JavaVM* jvm)
{
    assert(jvm != NULL);
    if (jvm == NULL)
    {
        return;
    }

    if (g_s_jvm == NULL)
    {
        g_s_jvm = jvm;
#if !defined(WIN32) && !defined(_WIN32_WCE)
        pthread_key_create(&g_s_key, &JniUtilCleanup_i);
#endif
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
    JniUtilStartup_i(jvm);

    return (JNI_VERSION_1_2);
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

#if !defined(WIN32) && !defined(_WIN32_WCE)
    env = (JNIEnv*)pthread_getspecific(g_s_key);
    if (env != NULL)
    {
        return (env);
    }
#endif

#if defined(ANDROID)
    if (g_s_jvm->AttachCurrentThread(&env, NULL) != JNI_OK || env == NULL)
    {
        return (NULL);
    }
#else
    if (g_s_jvm->AttachCurrentThread((void**)&env, NULL) != JNI_OK || env == NULL)
    {
        return (NULL);
    }
#endif

#if !defined(WIN32) && !defined(_WIN32_WCE)
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

#if defined(WIN32) || defined(_WIN32_WCE)
    g_s_jvm->DetachCurrentThread();
#endif
}

/////////////////////////////////////////////////////////////////////////////
////

#if defined(__cplusplus)
}
#endif
