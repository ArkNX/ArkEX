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

#pragma once

#include "base/AFSingleton.hpp"
#include "base/AFMap.hpp"
#include "interface/AFIPluginManager.h"
#include "AFCDynLib.h"

namespace ark
{

    class AFCPluginManager : public AFIPluginManager, public AFSingleton<AFCPluginManager>
    {
    public:
        AFCPluginManager();

        bool Init() override;
        bool PostInit() override;
        bool CheckConfig() override;
        bool PreUpdate() override;
        bool Update() override;
        bool PreShut() override;
        bool Shut() override;

        //////////////////////////////////////////////////////////////////////////

        void Register(AFIPlugin* pPlugin) override;
        void Deregister(AFIPlugin* pPlugin) override;
        //////////////////////////////////////////////////////////////////////////

        AFIPlugin* FindPlugin(const std::string& strPluginName) override;

        void AddModule(const std::string& strModuleName, AFIModule* pModule) override;
        void RemoveModule(const std::string& strModuleName) override;
        AFIModule* FindModule(const std::string& strModuleName) override;

        const std::string& AppName() const override;
        void SetAppName(const std::string& app_name) override;

        int64_t GetNowTime() const override;

        const std::string& GetResPath() const override;

        void SetPluginConf(const std::string& strFileName) override;

        void SetLogPath(const std::string& log_path) override;
        const std::string& GetLogPath() const override;

    protected:
        bool LoadPluginConf();

        //bool CreatePluginLogicThread(AFILogicThreadManager* logic_thread_manager, int thread_logic_id);

        bool LoadPluginLibrary(const std::string& strPluginDLLName);
        bool UnloadPluginLibrary(const std::string& plugin_name);

    private:
        //Current time(ms)
        int64_t cur_time_{ 0 };
        //plugin so/dll file path
        std::string plugin_path_{};
        //Resource path
        std::string res_path_{};
        //app.plugin file path
        std::string plugin_conf_path_{};
        //app name
        std::string app_name_{};
        //log output path
        std::string log_path_{};
        typedef void(*DLL_ENTRY_PLUGIN_FUNC)(AFIPluginManager* pm);
        typedef void(*DLL_EXIT_PLUGIN_FUNC)(AFIPluginManager* pm);

        std::map<std::string, bool> mxPluginNameMap;
        std::vector<std::string> mxPluginNameVec; // order
        AFMap<std::string, AFCDynLib> mxPluginLibMap;
        AFMap<std::string, AFIPlugin> mxPluginInstanceMap;
        AFMap<std::string, AFIModule> mxModuleInstanceMap;
        std::vector<AFIModule*> mxModuleInstanceVec; // order
    };

}