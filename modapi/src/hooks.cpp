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
#include "memoryutils.h"
#include "eventmanager.h"
#include "patches.h"
#include "abyssengine.h"
#include "hooks.h"
#include <Game/player.h>
#include <Game/system.h>
#include <Game/station.h>
#include <Game/mission.h>
#include <Game/asset.h>

Hooks::globals_init Hooks::oldglobals_init = nullptr;

void Hooks::injectsystems()
{
    auto* galaxy = *(Galaxy**)GLOBALS_GALAXY;
    
    if (!galaxy || !galaxy->systems) {
        std::cout << "[-] Failed to get Globals::Galaxy" << std::endl;
        return;
    }

    auto* systems = galaxy->systems;
    SingleSystem* wolf = nullptr;

    for (uint32_t i = 0; i < systems->size; i++)
        if (auto* s = systems->data[i]; s && s->name.text && wcscmp(s->name.text, L"Wolf-Reiser") == 0) {
            wolf = s;
            break; 
        }

    if (!wolf)
        return;

    SingleSystem copy = *wolf;
    copy.id = 27;
    copy.jumpgate_station_id = 30;
    copy.pos = { 80, 30, 47 };
    copy.starts_unlocked = true;
    copy.name = AbyssEngine::newstring(L"Test system");

    copy.linked_system_ids = AbyssEngine::newarray<uint32_t>(1);
    copy.linked_system_ids->data[0] = copy.id;

    copy.station_ids = AbyssEngine::newarray<uint32_t>(1);
    copy.station_ids->data[0] = 109;

    auto* allocsys = (SingleSystem*)VirtualAlloc(nullptr, sizeof(SingleSystem), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memcpy(allocsys, &copy, sizeof(SingleSystem));

    auto* newarr_sys = (AEArray<SingleSystem*>*)VirtualAlloc(nullptr, sizeof(AEArray<SingleSystem*>), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    newarr_sys->size = newarr_sys->size2 = systems->size + 1;
    newarr_sys->data = (SingleSystem**)VirtualAlloc(nullptr, sizeof(SingleSystem*) * newarr_sys->size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memcpy(newarr_sys->data, systems->data, sizeof(SingleSystem*) * systems->size);
    newarr_sys->data[systems->size] = allocsys;

    galaxy->systems = newarr_sys;

    Patches::patchstarmap(28);
    Patches::patchloadstations(110);
}

uintptr_t __stdcall Hooks::globals_init_hook(uintptr_t a, uintptr_t b, uintptr_t c)
{
    auto old = oldglobals_init(a, b, c);
    static bool one = false;
    if (!one) {
        one = true;
        CreateThread(nullptr, 0, [](LPVOID)->DWORD {
            std::cout << "[+] Globals::init hooked successfully" << std::endl;
            //Hooks::injectsystems();
            return 0;
        }, nullptr, 0, nullptr);
    }
    return old;
}

void Hooks::init()
{
    std::cout << "[*] Starting hooks..." << std::endl;
    MH_Initialize();
    MH_CreateHook((LPVOID)GLOBALS_INIT_ADDR, &globals_init_hook, (LPVOID*)&oldglobals_init);
    MH_EnableHook((LPVOID)GLOBALS_INIT_ADDR);
}