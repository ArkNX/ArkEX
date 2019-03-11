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

#include <args/args.hxx>
#include "base/AFMacros.hpp"
#include "base/AFDateTime.hpp"
#include "base/AFMisc.hpp"
#include "AFCPluginManager.h"

#include "../test/TestLogicThreadManager.h"
#include "../test/TestEventsManager.h"

using namespace ark;

bool g_exit_loop = false;
std::thread g_cmd_thread;

#if ARK_PLATFORM == PLATFORM_WIN

//mini-dump
void CreateDumpFile(const std::string& strDumpFilePathName, EXCEPTION_POINTERS* pException)
{
    HANDLE hDumpFile = CreateFile(strDumpFilePathName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
}

long ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
    AFDateTime now;
    std::string dump_name = ARK_FORMAT("{}-{:04d}{:02d}{:02d}_{:02d}_{:02d}_{:02d}.dmp",
                                       AFCPluginManager::get()->AppName(),
                                       now.GetYear(), now.GetMonth(), now.GetDay(),
                                       now.GetHour(), now.GetMinute(), now.GetSecond());

    CreateDumpFile(dump_name.c_str(), pException);

    FatalAppExit(-1, dump_name.c_str());
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void CloseXButton()
{
#if ARK_PLATFORM == PLATFORM_WIN
    HWND hWnd = GetConsoleWindow();

    if (hWnd)
    {
        HMENU hMenu = GetSystemMenu(hWnd, FALSE);
        EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND);
    }

#else
    //Do nothing
#endif
}

void InitDaemon()
{
#if ARK_PLATFORM == PLATFORM_UNIX
    int ret = daemon(1, 0);
    ARK_ASSERT_NO_EFFECT(ret == 0);

    // ignore signals
    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
#endif
}

void PrintLogo()
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif

    std::string logo = R"(
**********************************************************************
   ___                  _ _   _                        _         _    _   ___  __
  / _ \ _   _  __ _  __| | | | | _____  __            / \   _ __| | _| \ | \ \/ /
 | | | | | | |/ _` |/ _` | |_| |/ _ \ \/ /  _____    / _ \ | '__| |/ /  \| |\  / 
 | |_| | |_| | (_| | (_| |  _  |  __/>  <  |_____|  / ___ \| |  |   <| |\  |/  \
  \__\_\\__,_|\__,_|\__,_|_| |_|\___/_/\_\         /_/   \_\_|  |_|\_\_| \_/_/\_\

Copyright 2018 (c) QuadHex. All Rights Reserved.
Website: https://quadhex.io
Github:  https://github.com/QuadHex
Gitee:   https://gitee.com/QuadHex
**********************************************************************
)";

    CONSOLE_INFO_LOG << logo << std::endl;

#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

void ThreadFunc()
{
    while (!g_exit_loop)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::string s;
        std::cin >> s;
        s = AFMisc::ToLower(s);
        if (s == "exit")
        {
            g_exit_loop = true;
        }
    }
}

void CreateBackThread()
{
    g_cmd_thread = std::thread(std::bind(&ThreadFunc));
}

bool ParseArgs(int argc, char* argv[])
{
    args::ArgumentParser parser("This is ArkNX plugin loader argument tools", "If you have any questions, please report an issue in GitHub.");
    args::HelpFlag help(parser, "help", "Display the help menu", {'h', "help"});
    args::ActionFlag xbutton(parser, "close", "Close [x] button in Windows", { 'x' }, []()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
        CloseXButton();
#endif
    });

    args::ActionFlag daemon(parser, "daemon", "Run application as daemon", { 'd' }, []()
    {
#if ARK_PLATFORM == PLATFORM_UNIX
        InitDaemon();
#endif
    });

    args::ValueFlag<std::string> name(parser, "name", "Set application name", { 'n', "name" }, "my-server", args::Options::Required | args::Options::Single);
    args::ValueFlag<std::string> plugin_cfg(parser, "plugin config path", "Set application plugin config", { 'p', "plugin" }, "test.plugin", args::Options::Required | args::Options::Single);

    //start parse argument list
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        CONSOLE_ERROR_LOG << parser;
        return false;
    }
    catch (args::ParseError& e)
    {
        CONSOLE_ERROR_LOG << e.what() << std::endl;
        CONSOLE_ERROR_LOG << parser;
        return false;
    }
    catch (args::ValidationError& e)
    {
        CONSOLE_ERROR_LOG << e.what() << std::endl;
        CONSOLE_ERROR_LOG << parser;
        return false;
    }

    //Set app name
    if (name)
{
    AFCPluginManager::get()->SetAppName(name.Get());

        std::string process_name = ARK_FORMAT("{}", name.Get());
        //Set process name
#if ARK_PLATFORM == PLATFORM_WIN
        SetConsoleTitle(process_name.c_str());
#elif ARK_PLATFORM == PLATFORM_UNIX
        //setproctitle(process_name.c_str(), argc, argv);
#endif
    }
    else
    {
        return false;
    }

    //Create back thread, for some command
    CreateBackThread();

    return true;
}

void MainLoop()
{
#if ARK_PLATFORM == PLATFORM_WIN

    __try
    {
        AFCPluginManager::get()->Update();
    }
    __except (ApplicationCrashHandler(GetExceptionInformation()))
    {
        //Do nothing for now.
    }

#else
    AFCPluginManager::get()->Update();
#endif
}

int main(int argc, char* argv[])
{
	/*
    if (!ParseArgs(argc, argv))
    {
        CONSOLE_INFO_LOG << "Application parameter is invalid, please check it..." << std::endl;
        return 0;
    }

    PrintLogo();

    ARK_ASSERT_RET_VAL(AFCPluginManager::get()->Init(), -1);
    ARK_ASSERT_RET_VAL(AFCPluginManager::get()->PostInit(), -1);
    ARK_ASSERT_RET_VAL(AFCPluginManager::get()->CheckConfig(), -1);
    ARK_ASSERT_RET_VAL(AFCPluginManager::get()->PreUpdate(), -1);

    while (!g_exit_loop)
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            if (g_exit_loop)
            {
                break;
            }

            MainLoop();
        }
    }

    AFCPluginManager::get()->PreShut();
    AFCPluginManager::get()->Shut();

    g_cmd_thread.join();
	*/

	//test code
	AFCLogicThreadManager logic_thread_manager;
	AFCThreadEventsManager thread_event_manager;
	AFCPluginManager plug_manager;

	UnitTestLogicThreadManager(logic_thread_manager, 
		(AFIPluginManager*)&plug_manager,
		(AFIThreadEventsManager* )&thread_event_manager);

	UnitTestEventsManager(thread_event_manager, (AFIThreadWakeUp* )&logic_thread_manager);

	getchar();

    return 0;
}
