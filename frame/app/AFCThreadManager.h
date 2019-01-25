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

#ifndef _AFCTHREADMANAGER_H
#define _AFCTHREADMANAGER_H

#include "AFCThread.h"
#include <map>

using namespace std;


namespace ark
{
    typedef map<int, AFCThread*> mapThreadList;

    class AFIThreadManager
    {
    public:
        AFIThreadManager() {};
        virtual ~AFIThreadManager() {};

        virtual void CheckThreadList() = 0;
    };

    //run mian thread logic
#if ARK_PLATFORM == PLATFORM_WIN
    unsigned MainThreadCallbackRun(void* arg)
#else
    void* MainThreadCallbackRun(void* arg)
#endif
    {

    };


    class AFCThreadManager
    {
    public:
        AFCThreadManager();
        ~AFCThreadManager();

        void Init(int main_check_time_interval);

        bool CreateThread(int thread_logic_id, ThreadCallbackLogic thread_callback_logic, ThreadErrorLogic thread_callback_error, void* arg);

        bool KillThread(int thread_logic_id);

        bool IsAlive(int thread_logic_id);

        bool Pause(int thread_logic_id);

        bool Resume(int thread_logic_id);

        virtual void CheckThreadList();

    private:
        void Lock();

        void UnLock();

    private:
        mapThreadList thread_list_;
        ThreadMutex* main_thread_mutex_;
    };
}

#endif
