#include "memoryutils.h"
#include "abyssengine.h"
#include <Game/player.h>
#include <Game/system.h>
#include <Game/station.h>
#include <Game/mission.h>
#include <Game/asset.h>
#include <thread>
#include <chrono>

void Station::init()
{
    auto start = std::chrono::high_resolution_clock::now();
    uintptr_t base = MemoryUtils::GetModuleBase("GoF2.exe");
    
    while (globals_status == nullptr) {
        globals_status = *reinterpret_cast<Globals_status**>(base + 0x20AD6C); // Globals::status
        if (globals_status == nullptr)
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "[+] Station initialization took: " << duration << "ms" << std::endl;
}

int Station::getid()
{
    if (globals_status->m_pStationInfo == nullptr)
        return 0;
    return globals_status->m_pStationInfo->id;
}

void Station::setid(int value)
{
    if (globals_status->m_pStationInfo == nullptr)
        return;
    globals_status->m_pStationInfo->id = value;
}

std::string Station::getname()
{
    if (globals_status->m_pStationInfo->name.text == nullptr)
        return "";
    return MemoryUtils::ReadWideString(reinterpret_cast<uintptr_t>(globals_status->m_pStationInfo->name.text));
}

void Station::setname(const std::string value)
{
    if (globals_status->m_pStationInfo->name.text == nullptr)
        return;
    MemoryUtils::WriteWideString(reinterpret_cast<uintptr_t>(globals_status->m_pStationInfo->name.text), value);
}

int Station::gettechlevel()
{
    if (globals_status->m_pStationInfo == nullptr)
        return 0;
    return globals_status->m_pStationInfo->techlevel;
}

void Station::settechlevel(int value)
{
    if (globals_status->m_pStationInfo == nullptr)
        return;
    globals_status->m_pStationInfo->techlevel = value;
}

bool Station::isvoid(void)
{
    if (getid() == -1)
        return true;
    return false;
}

int Station::getangaritemscount()
{
    if (globals_status->m_pStationInfo->m_pItemsInAngar == nullptr)
        return 0;
    return globals_status->m_pStationInfo->m_pItemsInAngar->size;
}

void Station::setangaritemscount(int value)
{
    if (globals_status->m_pStationInfo->m_pItemsInAngar == nullptr)
        return;
    globals_status->m_pStationInfo->m_pItemsInAngar->size = value;
    globals_status->m_pStationInfo->m_pItemsInAngar->size2 = value;
}

int Station::getangarshipscount()
{
    if (globals_status->m_pStationInfo->m_pShipsInAngar == nullptr)
        return 0;
    return globals_status->m_pStationInfo->m_pShipsInAngar->size;
}

void Station::setangarshipscount(int value)
{
    if (globals_status->m_pStationInfo->m_pShipsInAngar == nullptr)
        return;
    globals_status->m_pStationInfo->m_pShipsInAngar->size = value;
    globals_status->m_pStationInfo->m_pShipsInAngar->size2 = value;
}

void Station::setangarshipid(int id, int value)
{
    if (globals_status->m_pStationInfo->m_pShipsInAngar == nullptr)
        return;
    unsigned int offset = (id == 0) ? 0 : (1 << (id + 1));
    auto* shipsarray = globals_status->m_pStationInfo->m_pShipsInAngar;
    uint8_t* arraydata = reinterpret_cast<uint8_t*>(shipsarray->data);
    SingleItem** ship_ptr = reinterpret_cast<SingleItem**>(arraydata + offset);
    
    if (*ship_ptr)
        (*ship_ptr)->m_nID = value;
}

int Station::getagentscount()
{
    if (globals_status->m_pStationInfo->m_pAgents == nullptr)
        return 0;
    return globals_status->m_pStationInfo->m_pAgents->size;
}

void Station::setagentscount(int value)
{
    if (globals_status->m_pStationInfo->m_pAgents == nullptr)
        return;
    globals_status->m_pStationInfo->m_pAgents->size = value;
    globals_status->m_pStationInfo->m_pAgents->size2 = value;
}

int Station::create(const std::string& str, int techlevel, int textureid, int systemid)
{
    if (EventManager::isearlyinit_finished) {
        std::cout << "[-] Failed to call station:Create(), you can only call it in the EarlyInit event" << std::endl;
        return -1;
    }
    SingleStation s;
    
    memset(&s, 0, sizeof(SingleStation));

    // utf8 to utf16 bruh I hate this game (sol2 can't pass a wchar directly -_-)
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring out(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, out.data(), len);

    s.name = AbyssEngine::newstring(out.c_str());
    s.id = created_stations.empty() ? 109 : 109 + created_stations.size();
    s.systemid = systemid;
    s.techlevel = techlevel;
    s.textureid = textureid;
    s.unk0 = 0; s.unk1 = 0; s.unk2 = 0;
    s.m_pShipsInAngar = 0;
    s.m_pItemsInAngar = 0;
    s.m_pAgents = 0;

    created_stations.push_back(s);
    return 108 + created_stations.size();
}

std::string Station::getagentname(int id)
{
    if (globals_status->m_pStationInfo->m_pAgents == nullptr)
        return "";
    SingleAgent** agents_list = reinterpret_cast<SingleAgent**>(globals_status->m_pStationInfo->m_pAgents->data);
    SingleAgent* agent = agents_list[id];
    return MemoryUtils::ReadWideString(reinterpret_cast<uintptr_t>(agent->m_sName.text));
}

int Station::getagentfaction(int id)
{
    if (globals_status->m_pStationInfo->m_pAgents == nullptr)
        return 0;
    SingleAgent** agents_list = reinterpret_cast<SingleAgent**>(globals_status->m_pStationInfo->m_pAgents->data);
    SingleAgent* agent = agents_list[id];
    return agent->m_nFactionType;
}

void Station::createagent(const std::string& name, int factiontype, int terranwoman)
{
    SingleStation* station = globals_status->m_pStationInfo;
    AEArray<SingleAgent*>* oldArray = reinterpret_cast<AEArray<SingleAgent*>*>(station->m_pAgents);
    uint32_t oldSize = (oldArray != nullptr) ? oldArray->size : 0;
    uint32_t newSize = oldSize + 1;
    AEArray<SingleAgent*>* newArray = AbyssEngine::newarray<SingleAgent*>(newSize);

    if (!newArray) return;
    if (oldArray != nullptr && oldArray->data != nullptr) {
        for (uint32_t i = 0; i < oldSize; ++i)
            newArray->data[i] = oldArray->data[i];
    }
    SingleAgent* pNewAgent = reinterpret_cast<SingleAgent*>(AbyssEngine::memory_allocate(sizeof(SingleAgent)));
    if (pNewAgent) {
        memset(pNewAgent, 0, sizeof(SingleAgent));
        int len = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, NULL, 0);
        wchar_t* buf = new wchar_t[len];
        MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, buf, len);
        pNewAgent->m_sName = AbyssEngine::newstring(buf);
        delete[] buf;
        pNewAgent->field_08 = 0;
        pNewAgent->field_0C = 0;
        pNewAgent->field_10 = 0;
        pNewAgent->field_14 = 0;         
        pNewAgent->field_18 = 0;
        pNewAgent->field_1C = 0;
        pNewAgent->m_nMissionStringLangId = 1059; // 803 funny
        pNewAgent->field_24 = -1;
        pNewAgent->field_28 = 0;
        pNewAgent->field_2C = 0;
        pNewAgent->m_nItemPrice = -1;
        pNewAgent->m_nItemStringLangId = -1;
        pNewAgent->field_38 = 0;
        pNewAgent->field_3C = 0;
        pNewAgent->m_nFactionType = factiontype;
        if (factiontype == 0 && terranwoman == 1)
            pNewAgent->m_nIsTerranWoman = 1;
        else
            pNewAgent->m_nIsTerranWoman = 0;
        pNewAgent->m_nGreetings = 1;
        pNewAgent->field_4C = 1;
        pNewAgent->m_nTalkingType = 1;
        pNewAgent->m_nAlliesPrice = 0;
        pNewAgent->field_58 = -1;
        pNewAgent->field_5C = -1;
        pNewAgent->field_60 = nullptr;
        pNewAgent->field_64 = 0;
        pNewAgent->field_68 = nullptr;
        pNewAgent->field_6C = 0;
        pNewAgent->field_70 = nullptr;
        pNewAgent->m_pAgentTextureFace = reinterpret_cast<AgentTextureFaceInfo*>(AbyssEngine::memory_allocate(sizeof(AgentTextureFaceInfo)));
        pNewAgent->m_pAgentTextureFace->faction_type = 0;
        pNewAgent->m_pAgentTextureFace->mouth = 0;
        pNewAgent->m_pAgentTextureFace->eyes = 0;
        pNewAgent->m_pAgentTextureFace->armor = 0;
        pNewAgent->m_pAgentTextureFace->hair = 0;
        pNewAgent->m_pAgentTextureFace->field_14 = 0;
        pNewAgent->m_pMissionInfo = nullptr;
        pNewAgent->m_pRecruitedAllies = nullptr;
        pNewAgent->field_80 = 0;
        newArray->data[oldSize] = pNewAgent;
    }
    station->m_pAgents = reinterpret_cast<AEArray<SingleAgent>*>(newArray);
}