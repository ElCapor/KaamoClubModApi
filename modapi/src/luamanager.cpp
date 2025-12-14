#include <windows.h>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <tlhelp32.h>
#include <vector>
#include <sol/sol.hpp>
#include <string>
#include "modapi_utils.h"
#include "luamanager.h"
#include "memoryutils.h"
#include <Game/player.h>

void LuaManager::init()
{
    lua_state.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math);
}

void LuaManager::bind_api()
{

    //lua_state.set_function("HelloWorld", &HelloWorld);
    
    lua_state.set_function("wait", [](int seconds) { 
        Sleep(seconds * 1000); 
    });

    lua_state.new_usertype<Player>("Player",
        sol::constructors<>(),
        "money", sol::property(&Player::getmoney, &Player::setmoney)
    );

    lua_state["API_VERSION"] = "1.0";
    lua_state["LocalPlayer"] = Player();
}

void LuaManager::execute_script(const std::string& filepath)
{
    try {
        auto result = lua_state.script_file(filepath);
        
        if (!result.valid()) {
            sol::error err = result;
            std::cout << "[LuaManager] Lua Script error: " << err.what() << std::endl;
        }
    }
    catch (const sol::error& e) {
        std::cout << "[LuaManager] Lua exception: " << e.what() << std::endl;
    }
}