#ifndef ITEM_H
#define ITEM_H
#include <windows.h>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <MinHook.h>
#include <filesystem>
#include <tlhelp32.h>
#include <vector>
#include <sol/sol.hpp>
#include <map>
#include <string>
#include "modapi_utils.h"
#include "luamanager.h"
#include "abyssengine.h"
#include "memoryutils.h"
#include "eventmanager.h"
#include <Game/player.h>
#include <Game/system.h>
#include <Game/station.h>
#include <Game/mission.h>
#include <Game/asset.h>

class Item {
    private:
        struct ModdedItems {
            int id;
            std::wstring name;
            std::wstring description;
            SingleItem item;
        };
    public:
        static inline std::vector<ModdedItems> created_items;
        static int create(const std::string& name, const std::string& description);
        static void edit(void);
        static void refreshitemsprices(void);
};
#endif