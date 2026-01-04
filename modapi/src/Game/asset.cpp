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
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "[+] Asset initialization took: " << duration << "ms" << std::endl;
}

std::string Asset::getassetfilepath(unsigned int id)
{
    return MemoryUtils::ReadString(globals_canvas->m_pResources.data[id]->m_pResourceInfo->m_sResourcePath);
}

void Asset::setassetfilepath(unsigned int id, const std::string value)
{
    MemoryUtils::WriteString(globals_canvas->m_pResources.data[id]->m_pResourceInfo->m_sResourcePath, value);
}