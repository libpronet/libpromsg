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

#if !defined(MSG_RECONNECTOR_H)
#define MSG_RECONNECTOR_H

#include "pronet/pro_memory_pool.h"
#include "pronet/pro_ref_count.h"
#include "pronet/pro_thread_mutex.h"
#include "pronet/pro_timer_factory.h"

/////////////////////////////////////////////////////////////////////////////
////

class CMsgClient;
class IProReactor;

/////////////////////////////////////////////////////////////////////////////
////

class CMsgReconnector : public IProOnTimer, public CProRefCount
{
public:

    static CMsgReconnector* CreateInstance();

    bool Init(
        CMsgClient*  client,
        IProReactor* reactor
        );

    void Fini();

    virtual unsigned long AddRef();

    virtual unsigned long Release();

    void Reconnect(unsigned int intervalInSeconds);

private:

    CMsgReconnector();

    virtual ~CMsgReconnector();

    virtual void OnTimer(
        void*    factory,
        uint64_t timerId,
        int64_t  tick,
        int64_t  userData
        );

private:

    CMsgClient*     m_client;
    IProReactor*    m_reactor;
    uint64_t        m_timerId;
    int64_t         m_connectTick;
    CProThreadMutex m_lock;

    DECLARE_SGI_POOL(0)
};

/////////////////////////////////////////////////////////////////////////////
////

#endif /* MSG_RECONNECTOR_H */
