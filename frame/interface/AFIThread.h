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

#ifndef _AFITHREAD_H
#define _AFITHREAD_H


namespace ark
{

    //Thread return
    enum ThreadReturn
    {
        ARK_THREAD_RETURN_ONCE = 0,
        ARK_THREAD_RETURN_CONTINUE,
        ARK_THREAD_RETURN_ERROR,
    };

    //Thread state
    enum ThreadState
    {
        ARK_THREAD_STATE_NONE = 0,
        ARK_THREAD_STATE_INIT,
        ARK_THREAD_STATE_LOGIC_RUN_BEGIN,
        ARK_THREAD_STATE_LOGIC_RUN_END,
        ARK_THREAD_STATE_LOGIC_ERROR,
        ARK_THREAD_STATE_LOGIC_CLOSE
    };

    enum ThreadError
    {
        ARK_THREAD_ERROR_CLOSE = 0,
        ARK_THREAD_ERROR_CONTINUE
    };

    enum ThreadLogicErrorType
    {
        ARK_THREAD_LOGIC_ERROR = 0,
        ARK_THREAD_LOGIC_TIMEOUT
    };

    class AFIThread
    {
    public:
        AFIThread() {};
        virtual ~AFIThread() {}

        virtual int GetThreadLogicID() = 0;

        virtual void Lock() = 0;

        virtual void UnLock() = 0;

        virtual void SaveLastRunTimeBegin() = 0;

        virtual void SaveLastRunTimeEnd() = 0;

        virtual void SetThreadState(ThreadState thread_state) = 0;

        virtual ThreadState GetThreadState() = 0;

        virtual AFIPluginManager* GetPluginManager() = 0;
    };

}

#endif //_AFITHREAD_H