#ifndef MODAPIUTILS_H
#define MODAPIUTILS_H
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

class ModApiUtils {
    private:
        DWORD getmainthreadid(void);
    public:
        void suspendgame(bool suspend);
        static void load_mods(LuaManager *luamanager);
};
#endif