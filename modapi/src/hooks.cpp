#include "patches.h"
#include "abyssengine.h"
#include "hooks.h"
#include <Game/system.h>
#include <iostream>
#include <vector>

Hooks::globals_init Hooks::oldglobals_init = nullptr;
Hooks::fileread_loadstationbinaryfromid Hooks::old_filereadloadstationbinaryfromid = nullptr;
Hooks::fileread_loadstationbinary Hooks::old_filereadloadstationbinary = nullptr;
Hooks::standing_isenemy Hooks::old_standingisenemy = nullptr;

void Hooks::injectitems()
{
    auto* itemsarray = (Items*)GLOBALS_ITEMS;
    if (!itemsarray || !itemsarray->items)
        return;
    auto* old_array = itemsarray->items;
    uint32_t old_count = old_array->size;
    uint32_t new_count = old_count + 1;
    auto* new_array = reinterpret_cast<AEArray<SingleItem*>*>(AbyssEngine::memory_allocate(sizeof(AEArray<SingleItem*>)));
    new_array->size = new_array->size2 = new_count;
    new_array->data = reinterpret_cast<SingleItem**>(AbyssEngine::memory_allocate(sizeof(SingleItem*) * new_count));
    memcpy(new_array->data, old_array->data, sizeof(SingleItem*) * old_count);
    if (old_count > 0) {
        SingleItem* first_item = old_array->data[195];
        auto* cloned_item = reinterpret_cast<SingleItem*>(AbyssEngine::memory_allocate(sizeof(SingleItem)));
        if (cloned_item) {
            memcpy(cloned_item, first_item, sizeof(SingleItem));
            cloned_item->m_nID = 196;
            new_array->data[old_count] = cloned_item;
        }
    }
    itemsarray->items = new_array;
    AbyssEngine::memory_free(old_array->data);
    AbyssEngine::memory_free(old_array);
}

void Hooks::injectsystemsandstations()
{
    auto* galaxy = *(Galaxy**)GLOBALS_GALAXY;
    if (!galaxy || !galaxy->systems || System::created_systems.empty())
        return;
    auto* systems = galaxy->systems;
    const uint32_t sys_count = systems->size;
    const uint32_t add_sys_count = (uint32_t)System::created_systems.size();
    const uint32_t new_sys_count = sys_count + add_sys_count;
    auto* new_sys_array = reinterpret_cast<AEArray<SingleSystem*>*>(AbyssEngine::memory_allocate(sizeof(AEArray<SingleSystem*>)));
    new_sys_array->size = new_sys_array->size2 = new_sys_count;
    new_sys_array->data = reinterpret_cast<SingleSystem**>(AbyssEngine::memory_allocate(sizeof(SingleSystem*) * new_sys_count));

    memcpy(new_sys_array->data, systems->data, sizeof(SingleSystem*) * sys_count);
    for (size_t i = 0; i < System::created_systems.size(); ++i) {
        const uint32_t sysid = sys_count + (uint32_t)i;
        const SingleSystem& src = System::created_systems[i];
        for (auto& st : Station::created_stations) {
            if (st.systemid == src.id)
                st.systemid = sysid;
        }
        auto* newsys = reinterpret_cast<SingleSystem*>(AbyssEngine::memory_allocate(sizeof(SingleSystem)));
        memset(newsys, 0, sizeof(SingleSystem));
        newsys->id = sysid;
        newsys->name = AbyssEngine::newstring(src.name.text); 
        newsys->pos = src.pos;
        newsys->risk = src.risk;
        newsys->faction = src.faction;
        newsys->texture_id = src.texture_id;
        newsys->starts_unlocked = true;
        std::vector<uint32_t> matching_ids;
        for (const auto& st : Station::created_stations) {
            if (st.systemid == sysid)
                matching_ids.push_back(st.id);
        }
        if (!matching_ids.empty()) {
            newsys->station_ids = AbyssEngine::newarray<uint32_t>((int)matching_ids.size());
            for (size_t j = 0; j < matching_ids.size(); ++j)
                newsys->station_ids->data[j] = matching_ids[j];
            if (src.linked_system_ids != nullptr)
                newsys->jumpgate_station_id = matching_ids[0];
        }
        uint32_t target_link_id = 0; 
        bool has_link = false;
        if (src.linked_system_ids != nullptr && src.linked_system_ids->size > 0) {
            newsys->linked_system_ids = AbyssEngine::newarray<uint32_t>(1);
            target_link_id = src.linked_system_ids->data[0];
            newsys->linked_system_ids->data[0] = target_link_id;
            has_link = true;
        }
        new_sys_array->data[sysid] = newsys;
        if (has_link && target_link_id < new_sys_count) {
            SingleSystem* target_sys = new_sys_array->data[target_link_id];
            if (target_sys) {
                uint32_t old_link_count = (target_sys->linked_system_ids) ? target_sys->linked_system_ids->size : 0;
                uint32_t new_link_count = old_link_count + 1;
                auto* new_links = AbyssEngine::newarray<uint32_t>(new_link_count);
                if (old_link_count > 0)
                    memcpy(new_links->data, target_sys->linked_system_ids->data, sizeof(uint32_t) * old_link_count);
                new_links->data[old_link_count] = sysid;
                target_sys->linked_system_ids = new_links;
            }
        }
    }
    galaxy->systems = new_sys_array;
    Patches::patchstarmap((uint8_t)new_sys_count); 
    System::created_systems.clear();
}

uintptr_t __stdcall Hooks::fileread_loadstationbinary_hook(SingleSystem* system)
{
    AEArray<SingleStation*>* old_array = reinterpret_cast<AEArray<SingleStation*>*>(old_filereadloadstationbinary(system));

    if (system) {
        std::vector<const SingleStation*> matchlist;

        for (const auto& st : Station::created_stations) {
            if (st.systemid == system->id) {
                matchlist.push_back(&st);
            }
        }
        if (!matchlist.empty()) {
            int count = (int)matchlist.size();
            auto* new_array = reinterpret_cast<AEArray<SingleStation*>*>(AbyssEngine::memory_allocate(sizeof(AEArray<SingleStation*>)));
            if (new_array) {
                new_array->data = reinterpret_cast<SingleStation**>(AbyssEngine::memory_allocate(sizeof(SingleStation*) * count));
                new_array->size = count; 
                new_array->size2 = count;

                for (size_t i = 0; i < matchlist.size(); ++i) {
                    auto* cloned = reinterpret_cast<SingleStation*>(AbyssEngine::memory_allocate(sizeof(SingleStation)));
                    if (cloned) {
                        memcpy(cloned, matchlist[i], sizeof(SingleStation));
                        cloned->name = AbyssEngine::newstring(matchlist[i]->name.text);
                    }
                    new_array->data[i] = cloned;
                }
                return reinterpret_cast<uintptr_t>(new_array);
            }
        }
    }
    return reinterpret_cast<uintptr_t>(old_array);
}

uintptr_t __stdcall Hooks::fileread_loadstationbinaryfromid_hook(const uint16_t* id)
{
    if (!id)
        return old_filereadloadstationbinaryfromid(id);
    uint32_t requested_id = (uint32_t)*id;

    for (const auto& st : Station::created_stations) {
        if (st.id == requested_id) {
            auto* new_array = reinterpret_cast<AEArray<SingleStation*>*>(AbyssEngine::memory_allocate(sizeof(AEArray<SingleStation*>)));
            if (new_array) {
                new_array->data = reinterpret_cast<SingleStation**>(AbyssEngine::memory_allocate(sizeof(SingleStation*) * 1));
                new_array->size = 1; 
                new_array->size2 = 1;
                auto* cloned = reinterpret_cast<SingleStation*>(AbyssEngine::memory_allocate(sizeof(SingleStation)));
                memcpy(cloned, &st, sizeof(SingleStation));
                new_array->data[0] = cloned;
                return reinterpret_cast<uintptr_t>(new_array);
            }
        }
    }
    return old_filereadloadstationbinaryfromid(id);
}

uintptr_t __stdcall Hooks::globals_init_hook(uintptr_t a, uintptr_t b, uintptr_t c)
{
    auto old = oldglobals_init(a, b, c);
    static bool called_once = false;

    if (!called_once) {
        called_once = true;
        CreateThread(nullptr, 0, [](LPVOID)->DWORD {
            Patches::patchmissions(100);
            //Hooks::injectitems();
            Hooks::injectsystemsandstations();
            return 0;
        }, nullptr, 0, nullptr);
    }
    return old;
}

bool __fastcall Hooks::standing_isenemy_hook(uintptr_t standing, int race)
{
    return true;
}

void Hooks::init()
{
    MH_Initialize();
    MH_CreateHook((LPVOID)GLOBALS_INIT_ADDR, &globals_init_hook, (LPVOID*)&oldglobals_init);
    MH_CreateHook((LPVOID)FILEREAD_LOADSTATIONBINARYFROMID, &fileread_loadstationbinaryfromid_hook, (LPVOID*)&old_filereadloadstationbinaryfromid);
    MH_CreateHook((LPVOID)FILEREAD_LOADSTATIONBIRARY, &fileread_loadstationbinary_hook, (LPVOID*)&old_filereadloadstationbinary);
    //MH_CreateHook((LPVOID)STANDING_ISENEMY, &standing_isenemy_hook, (LPVOID*)&old_standingisenemy);
    MH_EnableHook(MH_ALL_HOOKS);
}