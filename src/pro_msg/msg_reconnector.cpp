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

#include "msg_reconnector.h"
#include "msg_client.h"
#include "pronet/pro_memory_pool.h"
#include "pronet/pro_net.h"
#include "pronet/pro_ref_count.h"
#include "pronet/pro_thread_mutex.h"
#include "pronet/pro_time_util.h"
#include "pronet/pro_timer_factory.h"
#include "pronet/pro_z.h"
#include <cassert>

/////////////////////////////////////////////////////////////////////////////
////

CMsgReconnector*
CMsgReconnector::CreateInstance()
{
    CMsgReconnector* const reconnector = new CMsgReconnector;

    return (reconnector);
}

CMsgReconnector::CMsgReconnector()
{
    m_client      = NULL;
    m_reactor     = NULL;
    m_timerId     = 0;
    m_connectTick = 0;
}

CMsgReconnector::~CMsgReconnector()
{
    Fini();
}

bool
CMsgReconnector::Init(CMsgClient*  client,
                      IProReactor* reactor)
{
    assert(client != NULL);
    assert(reactor != NULL);
    if (client == NULL || reactor == NULL)
    {
        return (false);
    }

    {
        CProThreadMutexGuard mon(m_lock);

        assert(m_client == NULL);
        assert(m_reactor == NULL);
        if (m_client != NULL || m_reactor != NULL)
        {
            return (false);
        }

        client->AddRef();
        m_client      = client;
        m_reactor     = reactor;
        m_connectTick = ProGetTickCount64();
    }

    return (true);
}

void
CMsgReconnector::Fini()
{
    CMsgClient* client = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_client == NULL || m_reactor == NULL)
        {
            return;
        }

        m_reactor->CancelTimer(m_timerId);
        m_timerId = 0;

        m_reactor = NULL;
        client = m_client;
        m_client = NULL;
    }

    client->Release();
}

unsigned long
PRO_CALLTYPE
CMsgReconnector::AddRef()
{
    const unsigned long refCount = CProRefCount::AddRef();

    return (refCount);
}

unsigned long
PRO_CALLTYPE
CMsgReconnector::Release()
{
    const unsigned long refCount = CProRefCount::Release();

    return (refCount);
}

void
CMsgReconnector::Reconnect()
{
    {
        CProThreadMutexGuard mon(m_lock);

        if (m_client == NULL || m_reactor == NULL)
        {
            return;
        }

        m_reactor->CancelTimer(m_timerId);
        m_timerId = 0;

        PRO_INT64 tickInterval =
            m_client->m_msgConfigInfo.msgc_reconnect_interval;
        tickInterval *= 1000;

        PRO_INT64 tickDelay =
            m_connectTick + tickInterval - ProGetTickCount64();
        if (tickDelay < 0)
        {
            tickDelay = 0;
        }

        m_timerId = m_reactor->ScheduleTimer(this, tickDelay, false);
    }
}

void
PRO_CALLTYPE
CMsgReconnector::OnTimer(unsigned long timerId,
                         PRO_INT64     userData)
{
    assert(timerId > 0);
    if (timerId == 0)
    {
        return;
    }

    CMsgClient* client = NULL;

    {
        CProThreadMutexGuard mon(m_lock);

        if (m_client == NULL || m_reactor == NULL)
        {
            return;
        }

        if (timerId != m_timerId)
        {
            return;
        }

        m_reactor->CancelTimer(m_timerId);
        m_timerId = 0;

        m_connectTick = ProGetTickCount64();

        m_client->AddRef();
        client = m_client;
    }

    client->Reconnect_i();
    client->Release();
}
