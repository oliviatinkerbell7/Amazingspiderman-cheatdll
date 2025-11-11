#include <windows.h>
#include <fstream>
#include "cheats.h"

// Thread that writes cheats
DWORD WINAPI CheatThread(LPVOID lpParam) {
    // Log file
    std::ofstream log(LOGFILE, std::ios::app);
    log << "DLL successfully hooked!\n";
    log.close();

    // Wait for GameLogic.dll to load
    HMODULE hMod = NULL;
    while (!(hMod = GetModuleHandleA("GameLogic.dll"))) {
        Sleep(100);
    }

    // Calculate addresses
    volatile int* health = (int*)((uintptr_t)hMod + OFF_HEALTH);
    volatile int* xp     = (int*)((uintptr_t)hMod + OFF_XP);
    volatile int* tech   = (int*)((uintptr_t)hMod + OFF_TECH);

    // Notify user
    MessageBoxA(NULL, "Progamingidea hook is running!", "DLL Loaded", MB_OK);

    // Main cheat loop
    while (true) {
        if (health) *health = 9999;
        if (xp) *xp = 9999;
        if (tech) *tech = 9999;
        Sleep(100); // 0.1 sec
    }

    return 0;
}

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        HANDLE hThread = CreateThread(NULL, 0, CheatThread, NULL, 0, NULL);
        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}
