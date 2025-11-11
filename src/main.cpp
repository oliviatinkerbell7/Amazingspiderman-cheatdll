#include <windows.h>
#include <fstream>
#include "../include/cheats.h"

// Thread for cheat logic
DWORD WINAPI CheatThread(LPVOID lpParam) {
    std::ofstream log(LOGFILE, std::ios::app);
    log << "[+] DLL successfully hooked!\n";
    log.close();

    // Wait until GameLogic.dll is loaded
    HMODULE hMod = nullptr;
    while (!(hMod = GetModuleHandleA("GameLogic.dll"))) {
        Sleep(100);
    }

    volatile int* health = (int*)((uintptr_t)hMod + OFF_HEALTH);
    volatile int* xp     = (int*)((uintptr_t)hMod + OFF_XP);
    volatile int* tech   = (int*)((uintptr_t)hMod + OFF_TECH);

    MessageBoxA(NULL, "Cheat DLL is running!", "Injected", MB_OK);

    while (true) {
        if (health) *health = 9999;
        if (xp) *xp = 9999;
        if (tech) *tech = 9999;
        Sleep(100);
    }

    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        CreateThread(NULL, 0, CheatThread, NULL, 0, NULL);
    }
    return TRUE;
}
