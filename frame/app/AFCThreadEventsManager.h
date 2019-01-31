/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#ifndef _AFCTHREADEVENTSMANAGER_H
#define _AFCTHREADEVENTSMANAGER_H

#include "base/AFMacros.hpp"
#include "interface/AFIMaintainThreadManager.h"
#include "interface/AFIThread.h"
#include "AFCThreadEvent.h"
#include <map>
#include <deque>

using namespace std;

namespace ark
{
    class AFCThreadEventsManager : public AFIMaintainThreadManager
    {
    public:
        AFCThreadEventsManager();
        ~AFCThreadEventsManager();

        void Close();

        void Init(int max_thread_events_count);

        virtual void CheckThreadList();

        virtual int64_t GetMainThreadCheckInterval();

        bool AddEvent(int thread_logic_id, AFCThreadEvent thread_event_);

        bool GetEvent(int thread_logic_id, AFCThreadEvent& thread_event);

    private:
        void Lock();

        void UnLock();

    private:
        typedef deque<AFCThreadEvent> vecEventList;
        typedef map<int, vecEventList*> mapThreadEvents;

    private:
        int             max_thread_events_count_;
        int64_t         main_check_time_interval_;
        mapThreadEvents map_thread_events_;
        ThreadMutex*    events_thread_mutex_;
    };
}

#endif