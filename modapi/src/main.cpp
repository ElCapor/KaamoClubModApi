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

DWORD WINAPI MainThread(LPVOID lpParam) {
    LuaManager *luamanager = new LuaManager();
    FILE* dummyfile;
    freopen_s(&dummyfile, "CONOUT$", "w", stdout);
    freopen_s(&dummyfile, "CONOUT$", "w", stderr);
    
    std::cout << "[+] KaamoClubModAPI Loaded! | Version: 1.0" << std::endl;
    Player::init();
    luamanager->init();
    luamanager->bind_api();
    ModApiUtils::load_mods(luamanager);
    
    while (true) {
        
    }

    if (dummyfile) fclose(dummyfile);
    FreeLibraryAndExitThread((HMODULE)lpParam, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);        
        HANDLE hThread = CreateThread(NULL, 0, MainThread, hModule, 0, NULL);
        if (hThread) CloseHandle(hThread);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}