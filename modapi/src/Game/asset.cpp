#include "memoryutils.h"
#include <Game/player.h>
#include <Game/system.h>
#include <Game/station.h>
#include <Game/mission.h>
#include <Game/asset.h>
#include <thread>
#include <chrono>

void Asset::init()
{
    auto start = std::chrono::high_resolution_clock::now();
    uintptr_t base = MemoryUtils::GetModuleBase("GoF2.exe");

    while (globals_canvas == nullptr) {
        globals_canvas = *reinterpret_cast<Globals_Canvas**>(base + 0x20AE68); // Globals::Canvas
        if (globals_canvas == nullptr) 
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

std::string Asset::getassetfilepath(unsigned int id)
{
    return MemoryUtils::ReadString(globals_canvas->m_pResources.data[id]->m_pResourceInfo->m_sResourcePath);
}

void Asset::setassetfilepath(unsigned int id, const std::string value)
{
    MemoryUtils::WriteString(globals_canvas->m_pResources.data[id]->m_pResourceInfo->m_sResourcePath, value);
}

std::string Asset::gettext(int id)
{
    AEString* returned_str = nullptr;
    uintptr_t address = 0x004F38B0;

    __asm {
        mov eax, id
        call address
        mov returned_str, eax
    }
    return ModApiUtils::w2s(returned_str->text);
}