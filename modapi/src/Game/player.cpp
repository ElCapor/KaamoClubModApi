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

void Player::init()
{
    money = MemoryUtils::GetModuleBase("GoF2.exe") + 0x20AD6C;
}

int Player::getmoney()
{
    uintptr_t finaladdr = MemoryUtils::GetPointerAddress(money, {0x174});
    return MemoryUtils::Read<int>(finaladdr);
}

void Player::setmoney(int value)
{
    uintptr_t finaladdr = MemoryUtils::GetPointerAddress(money, {0x174});
    MemoryUtils::Write<int>(finaladdr, value);
}