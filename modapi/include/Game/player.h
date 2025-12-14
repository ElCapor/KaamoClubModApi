#ifndef PLAYER_H
#define PLAYER_H
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

class Player {
    private:
        static inline uintptr_t money = 0;
    public:
        static void init(void);
        static int getmoney(void);
        static void setmoney(int value);
};
#endif