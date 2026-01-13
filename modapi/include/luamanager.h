#ifndef LUAMANAGER_H
#define LUAMANAGER_H
#include <windows.h>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <tlhelp32.h>
#include <vector>
#include <sol/sol.hpp>
#include <map>
#include <string>
#include "modapi_utils.h"
#include "luamanager.h"
#include "memoryutils.h"
#include "eventmanager.h"
#include <Game/player.h>
#include <Game/system.h>
#include <Game/station.h>
#include <Game/mission.h>
#include <Game/asset.h>

class LuaManager {
    private:
        sol::state lua_state;
    public:
        void init(void);
        void bind_api(void);
        void execute_script(const std::string& filepath);
        lua_State* getluastate(void);
};
#endif