#include <windows.h>
#include <fstream>
#include <stdint.h>

DWORD WINAPI CheatThread(LPVOID lpParam) {
    std::ofstream log("C:\\Users\\Public\\hook_log.txt", std::ios::app);
    log << "DLL successfully hooked!\n";
    log.close();

    HMODULE hMod = GetModuleHandle(L"GameLogic.dll");
    if (!hMod) {
        while (!(hMod = GetModuleHandle(L"GameLogic.dll"))) {
            Sleep(1000);
        }
    }

    volatile int* health = (int*)((uintptr_t)hMod + 0x4E2230);
    volatile int* xp     = (int*)((uintptr_t)hMod + 0x4F7B74);
    volatile int* tech   = (int*)((uintptr_t)hMod + 0x4F7B78);

    MessageBoxA(NULL, "Progamingidea hook is running!", "DLL Loaded", MB_OK);

    while (true) {
        try {
            if (health) *health = 9999;
            if (xp) *xp = 9999;
            if (tech) *tech = 9999;
        } catch (...) {
            // ignore any memory write errors
        }
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
