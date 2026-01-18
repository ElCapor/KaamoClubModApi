#include <windows.h>
#include <iostream>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <tlhelp32.h>
#include <vector>
#include <codecvt>
#include <sol/sol.hpp>
#include <map>
#include <string>
#include "modapi_utils.h"
#include "luamanager.h"
#include "memoryutils.h"
#include "eventmanager.h"
#include "abyssengine.h"
#include <Game/player.h>
#include <Game/system.h>
#include <Game/station.h>
#include <Game/mission.h>
#include <Game/asset.h>

DWORD ModApiUtils::getmainthreadid() 
{
    DWORD pid = GetCurrentProcessId();
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    THREADENTRY32 te32;
    DWORD mainthreadid = 0;
    unsigned long long oldestTime = (unsigned long long)-1;

    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    te32.dwSize = sizeof(THREADENTRY32);
    if (Thread32First(hSnapshot, &te32)) {
        do {
            if (te32.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
                if (hThread) {
                    FILETIME ftCreation, ftExit, ftKernel, ftUser;
                    if (GetThreadTimes(hThread, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
                        unsigned long long time = ((unsigned long long)ftCreation.dwHighDateTime << 32) | ftCreation.dwLowDateTime;
                        if (time < oldestTime) {
                            oldestTime = time;
                            mainthreadid = te32.th32ThreadID;
                        }
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te32));
    }
    CloseHandle(hSnapshot);
    return mainthreadid;
}

void ModApiUtils::suspendgame(bool suspend)
{
    DWORD mainthreadid = getmainthreadid();

    if (mainthreadid == 0)
        return;

    HANDLE hMainThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, mainthreadid);

    if (hMainThread) {
        if (suspend)
            SuspendThread(hMainThread);
        else 
            while (ResumeThread(hMainThread) > 1);
        CloseHandle(hMainThread);
    } else {
        std::cout << "[-] Couldn't open main game thread??? black magic????" << std::endl;
    }
}

void ModApiUtils::load_mods(LuaManager *luamanager)
{
    std::string mods_folder = "mods";
    
    // TODO: make a folder lol
    if (!std::filesystem::exists(mods_folder) || !std::filesystem::is_directory(mods_folder)) {
        std::cout << "[-] Mods folder not found: " << mods_folder << std::endl;
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(mods_folder)) {
        if (entry.is_directory()) {
            std::string mod_path = entry.path().string();
            std::string init_lua = mod_path + "/init.lua";
            
            if (std::filesystem::exists(init_lua)) {
                std::cout << "[*] Loading mod: " << entry.path().filename().string() << std::endl;
                
                luamanager->execute_script(init_lua);
            }
        }
    }    
}

// credits https://gist.github.com/danzek/d6a0e4a48a5439e7f808ed1497f6268e
std::string ModApiUtils::w2s(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}

// TODO: create items.cpp and put it in items it will be cleaner I guess
void ModApiUtils::refreshitemsprices()
{
    Globals_status** globals_status_ptr = (Globals_status**)0x0060AD6C;
    Globals_status* globals_status = *globals_status_ptr;

    AEArray<unsigned int>* oldlowprices = globals_status->m_pItemLowestPrices;
    AEArray<unsigned int>* newlowprices = (AEArray<unsigned int>*)AbyssEngine::memory_allocate(sizeof(AEArray<unsigned int>));
    newlowprices->size = 197;
    newlowprices->size2 = 197;
    newlowprices->data = (unsigned int*)AbyssEngine::memory_allocate(sizeof(unsigned int) * 197);
    memcpy(newlowprices->data, oldlowprices->data, sizeof(unsigned int) * oldlowprices->size);
    newlowprices->data[196] = 72500; // newitem->m_nMinPrice
    globals_status->m_pItemLowestPrices = newlowprices;
    AbyssEngine::memory_free(oldlowprices->data);
    AbyssEngine::memory_free(oldlowprices);
    
    AEArray<unsigned int>* oldhighprices = globals_status->m_pItemHighestPrices;
    AEArray<unsigned int>* newhighprices = (AEArray<unsigned int>*)AbyssEngine::memory_allocate(sizeof(AEArray<unsigned int>));
    newhighprices->size = 197;
    newhighprices->size2 = 197;
    newhighprices->data = (unsigned int*)AbyssEngine::memory_allocate(sizeof(unsigned int) * 197);
    memcpy(newhighprices->data, oldhighprices->data, sizeof(unsigned int) * oldhighprices->size);
    newhighprices->data[196] = 83375; // newitem->m_nMaxPrice
    globals_status->m_pItemHighestPrices = newhighprices;
    AbyssEngine::memory_free(oldhighprices->data);
    AbyssEngine::memory_free(oldhighprices);

    AEArray<unsigned int>* oldlowprices_sys = globals_status->m_pItemLowestPricesSystem;
    AEArray<unsigned int>* newlowprices_sys = (AEArray<unsigned int>*)AbyssEngine::memory_allocate(sizeof(AEArray<unsigned int>));
    newlowprices_sys->size = 197;
    newlowprices_sys->size2 = 197;
    newlowprices_sys->data = (unsigned int*)AbyssEngine::memory_allocate(sizeof(unsigned int) * 197);
    memcpy(newlowprices_sys->data, oldlowprices_sys->data, sizeof(unsigned int) * oldlowprices_sys->size);
    newlowprices_sys->data[196] = 10; // newitem->m_nLowestPriceSystemId
    globals_status->m_pItemLowestPricesSystem = newlowprices_sys;
    AbyssEngine::memory_free(oldlowprices_sys->data);
    AbyssEngine::memory_free(oldlowprices_sys);

    AEArray<unsigned int>* oldhighprices_sys = globals_status->m_pItemHighestPricesSystem;
    AEArray<unsigned int>* newhighprices_sys = (AEArray<unsigned int>*)AbyssEngine::memory_allocate(sizeof(AEArray<unsigned int>));
    newhighprices_sys->size = 197;
    newhighprices_sys->size2 = 197;
    newhighprices_sys->data = (unsigned int*)AbyssEngine::memory_allocate(sizeof(unsigned int) * 197);
    memcpy(newhighprices_sys->data, oldhighprices_sys->data, sizeof(unsigned int) * oldhighprices_sys->size);
    newhighprices_sys->data[196] = 4; // newitem->m_nHighestPriceSystemId
    globals_status->m_pItemHighestPricesSystem = newhighprices_sys;
    AbyssEngine::memory_free(oldhighprices_sys->data);
    AbyssEngine::memory_free(oldhighprices_sys);
}