#ifndef HOOKS_H
#define HOOKS_H
#include <Game/structs.h>

class Hooks {
    private:
        static constexpr uintptr_t GLOBALS_INIT_ADDR = 0x0044B20C;
        static constexpr uintptr_t GLOBALS_GALAXY = 0x0060AF3C;
        static constexpr uintptr_t GLOBALS_ITEMS = 0x0060AEA4;
        static constexpr uintptr_t FILEREAD_LOADSTATIONBINARYFROMID = 0x408880;
        static constexpr uintptr_t FILEREAD_LOADSTATIONBIRARY = 0x408C4B;
        static constexpr uintptr_t STANDING_ISENEMY = 0x4CE34B;
        static constexpr uintptr_t ABYSSENGINE_PAINTCANVAS_SETCOLOR = 0x4EC3E0;
        static constexpr uintptr_t GAMETEXT_GETTEXT = 0x4F38B0;
        static constexpr uintptr_t RECORDHANDLER_RECORDSTOREWRITE = 0x004BF40C;

        using globals_init = uintptr_t (__stdcall*)(uintptr_t, uintptr_t, uintptr_t);
        static globals_init oldglobals_init;
        static uintptr_t __stdcall globals_init_hook(uintptr_t a, uintptr_t b, uintptr_t c);
        
        using fileread_loadstationbinaryfromid = uintptr_t(__stdcall*)(const uint16_t* id);
        static fileread_loadstationbinaryfromid old_filereadloadstationbinaryfromid;
        static uintptr_t __stdcall fileread_loadstationbinaryfromid_hook(const uint16_t* id);

        using fileread_loadstationbinary = uintptr_t(__stdcall*)(SingleSystem *system);
        static fileread_loadstationbinary old_filereadloadstationbinary;
        static uintptr_t __stdcall fileread_loadstationbinary_hook(SingleSystem *system);

        using standing_isenemy = bool (__fastcall*)(uintptr_t standing, int race);
        static standing_isenemy old_standingisenemy;
        static bool __fastcall standing_isenemy_hook(uintptr_t standing, int race);

        using abyssengine_paintcanvas_setcolor = void (__fastcall*)(uintptr_t paintcanvas);
        static abyssengine_paintcanvas_setcolor old_abyssenginepaintcanvassetcolor;
        static void __fastcall abyssengine_paintcanvas_setcolor_hook(uintptr_t paintcanvas);

        using gametext_gettext = AEString* (__fastcall*)(void);
        static gametext_gettext old_gametextgettext;
        static AEString* __fastcall gametext_gettext_hook(void);

        using recordhandler_recordstorewrite = int (__stdcall*)(uintptr_t a, int b);
        static recordhandler_recordstorewrite old_recordhandlerrecordstorewrite;
        static int __stdcall recordhandler_recordstorewrite_hook(uintptr_t a, int b);

        static void injectsystemsandstations(void);
        static void injectitems(void);
    public:
        static void init(void);
};

#endif