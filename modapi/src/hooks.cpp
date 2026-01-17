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
Hooks::abyssengine_paintcanvas_setcolor Hooks::old_abyssenginepaintcanvassetcolor = nullptr;

void Hooks::injectitems()
{
    Items* itemsarray = (Items*)GLOBALS_ITEMS;
    if (!itemsarray || !itemsarray->items)
        return;
    AEArray<SingleItem*>* old_array = itemsarray->items;
    int newcount = old_array->size + 1;
    AEArray<SingleItem*>* new_array = reinterpret_cast<AEArray<SingleItem*>*>(AbyssEngine::memory_allocate(sizeof(AEArray<SingleItem*>)));
    new_array->size = newcount;
    new_array->size2 = newcount;
    new_array->data = reinterpret_cast<SingleItem**>(AbyssEngine::memory_allocate(sizeof(SingleItem*) * newcount));
    memset(new_array->data, 0, sizeof(SingleItem*) * newcount);
    memcpy(new_array->data, old_array->data, sizeof(SingleItem*) * old_array->size);
    SingleItem* newitem = reinterpret_cast<SingleItem*>(AbyssEngine::memory_allocate(sizeof(SingleItem)));
    newitem->m_nID = 196;
    newitem->m_nType = 3;
    newitem->m_nSubType = 14;
    newitem->m_nTechLevel = 8;
    newitem->m_nLowestPriceSystemId = 0; // KEEP IT TO 0 OR ELSE THE GAME GOES BOOM BOOM ON CUSTOM AGENTS
    newitem->m_nHighestPriceSystemId = 0; // KEEP IT TO 0 OR ELSE THE GAME GOES BOOM BOOM ON CUSTOM AGENTS
    newitem->m_nPrice = 80000;
    newitem->m_nOccurance = 0;
    newitem->m_nMinPrice = 70000;
    newitem->m_nMaxPrice = 90000;
    newitem->m_nAmount = 0;
    newitem->m_nStationAmount = 0;
    newitem->field_34 = 0;
    newitem->field_38 = 0;
    newitem->field_3C = 0;
    newitem->field_40 = 0;

    AEArray<ItemInfo>* iteminfoarr = reinterpret_cast<AEArray<ItemInfo>*>(AbyssEngine::memory_allocate(sizeof(AEArray<ItemInfo>)));
    iteminfoarr->size = 28;
    iteminfoarr->size2 = 28;
    iteminfoarr->data = reinterpret_cast<ItemInfo*>(AbyssEngine::memory_allocate(sizeof(ItemInfo)));
    memset(iteminfoarr->data, 0, iteminfoarr->size);
    //memcpy(iteminfoarr->data, old_array->data[195]->m_pItemInfo, sizeof(ItemInfo) * 28);
    ItemInfo* info = iteminfoarr->data; 
    info->field_0 = 0;
    info->m_nID = 196;
    info->m_bIsThermo = 1; 
    info->m_nType = 3;
    info->field_10 = 2;
    info->m_nSubType = 14;
    info->field_18 = 3;
    info->m_nTechLevel = 8;
    info->m_nHighestPriceSystemId = 1;
    info->m_nLowestPriceSystemId = 1;
    info->field_28 = 5;
    info->field_2C = 4;
    info->field_30 = 6;
    info->field_34 = 0;
    info->field_38 = 7;
    info->field_3C = 72500;
    info->field_40 = 8;
    info->m_nPrice = 80000;
    info->field_48 = 25;
    info->m_nEffect = 300;
    info->m_nPropertyOne = 26;
    info->m_nLoadingSpeed = 100;
    info->m_nPropertyTwo = 27;
    info->m_nBoostDuration = 90000;
    info->m_nPropertyThree = 44;
    info->field_64 = 0;
    info->m_nPropertyFour = 45;
    info->field_6C = 107;
    
    //iteminfoarr->data = iteminfo;
    newitem->m_pItemInfo = iteminfoarr;

    new_array->data[196] = newitem;
    itemsarray->items = new_array;
    // I'm not freeing old_array->data[i]->m_pItemInfo array because I did a memcpy of old array to new array and I don't want to realloc every item info it's kinda useless
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

void __fastcall Hooks::abyssengine_paintcanvas_setcolor_hook(uintptr_t paintcanvas)
{
    unsigned int hexcolor;

    // the game puts the color in eax......
    __asm {
        mov hexcolor, eax
    }
    //hexcolor = 0x00ff00ff; // r,g,b,a | ff = 255
    __asm {
        mov ecx, paintcanvas
        mov eax, hexcolor
        call old_abyssenginepaintcanvassetcolor
    }
}

void Hooks::init()
{
    MH_Initialize();
    MH_CreateHook((LPVOID)GLOBALS_INIT_ADDR, &globals_init_hook, (LPVOID*)&oldglobals_init);
    MH_CreateHook((LPVOID)FILEREAD_LOADSTATIONBINARYFROMID, &fileread_loadstationbinaryfromid_hook, (LPVOID*)&old_filereadloadstationbinaryfromid);
    MH_CreateHook((LPVOID)FILEREAD_LOADSTATIONBIRARY, &fileread_loadstationbinary_hook, (LPVOID*)&old_filereadloadstationbinary);
    //MH_CreateHook((LPVOID)STANDING_ISENEMY, &standing_isenemy_hook, (LPVOID*)&old_standingisenemy);
    MH_CreateHook((LPVOID)ABYSSENGINE_PAINTCANVAS_SETCOLOR, &abyssengine_paintcanvas_setcolor_hook, (LPVOID*)&old_abyssenginepaintcanvassetcolor);
    MH_EnableHook(MH_ALL_HOOKS);
}