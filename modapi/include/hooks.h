#ifndef HOOKS_H
#define HOOKS_H

class Hooks {
    private:
        static constexpr uintptr_t GLOBALS_INIT_ADDR = 0x0044B20C;
        static constexpr uintptr_t GLOBALS_GALAXY = 0x0060AF3C;
        static constexpr uintptr_t FILEREAD_LOADSTATIONBINARYFROMID = 0x408880;

        using globals_init = uintptr_t (__stdcall*)(uintptr_t, uintptr_t, uintptr_t);
        static globals_init oldglobals_init;
        static uintptr_t __stdcall globals_init_hook(uintptr_t a, uintptr_t b, uintptr_t c);
        
        using fileread_loadstationbinaryfromid = uintptr_t (__stdcall*)(const uint16_t* a);
        static fileread_loadstationbinaryfromid old_filereadloadstationbinaryfromid;
        static uintptr_t __stdcall fileread_loadstationbinaryfromid_hook(const uint16_t* a);

        static void injectsystems(void);
    public:
        static void init(void);
};

#endif