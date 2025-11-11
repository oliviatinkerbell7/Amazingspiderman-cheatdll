#include <windows.h>
#include <fstream>
#include "cheats.h"

DWORD WINAPI CheatThread(LPVOID lpParam) {
    std::ofstream log("C:\\Users\\Public\\hook_log.txt", std::ios::app);
    log << "DLL successfully hooked!\n";
    log.close();

    // Use ANSI version since MinGW expects LPCSTR, not LPCWSTR
    HMODULE hGameLogic = GetModuleHandleA("GameLogic.dll");
    while (!hGameLogic) {
        Sleep(100);
        hGameLogic = GetModuleHandleA("GameLogic.dll");
    }

    volatile int* health = (int*)((uintptr_t)hGameLogic + HEALTH_ADDR);
    volatile int* xp     = (int*)((uintptr_t)hGameLogic + XP_ADDR);
    volatile int* tech   = (int*)((uintptr_t)hGameLogic + TECH_ADDR);

    MessageBoxA(NULL, "Progamingidea hook is running!", "DLL Loaded", MB_OK);

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
        HANDLE hThread = CreateThread(NULL, 0, CheatThread, NULL, 0, NULL);
        if (hThread) CloseHandle(hThread);
    }
    return TRUE;
}
