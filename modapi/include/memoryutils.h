#ifndef MEMORYUTILS_H
#define MEMORYUTILS_H
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

class MemoryUtils {
    public:
        static uintptr_t GetModuleBase(const char* modulename);
        static uintptr_t GetPointerAddress(uintptr_t baseaddr, const std::vector<unsigned int>& offsets);

        template <typename T>
        static T Read(uintptr_t address) {
            T buffer;
            ReadProcessMemory(GetCurrentProcess(), (LPCVOID)address, &buffer, sizeof(T), NULL);
            return buffer;
        }

        template <typename T>
        static bool Write(uintptr_t address, T value) {
            return WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, &value, sizeof(T), NULL);
        }
};
#endif