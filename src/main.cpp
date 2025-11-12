#include <windows.h>
#include <fstream>
#include <cstdint>
#include "cheats.h"

// Helper: safe memory check before writing
static bool memory_is_writeable(void* addr) {
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(addr, &mbi, sizeof(mbi)) == 0) return false;
    if (mbi.State != MEM_COMMIT) return false;
    DWORD prot = mbi.Protect;
    // Exclude guard/no access pages
    if (prot & (PAGE_NOACCESS | PAGE_GUARD)) return false;
    // If it's read-only (no write) don't write
    if (prot & (PAGE_READONLY)) return false;
    return true;
}

static void write_log(const char* s) {
    std::ofstream log(LOGFILE, std::ios::app);
    if (log) {
        log << s << std::endl;
    }
}

// Thread that writes cheats
DWORD WINAPI CheatThread(LPVOID lpParam) {
    write_log("CheatThread started.");

    // Wait for GameLogic.dll to load
    HMODULE hMod = NULL;
    for (int i=0; i<300 && !hMod; ++i) { // try ~30s total
        hMod = GetModuleHandleA("GameLogic.dll");
        if (!hMod) Sleep(100);
    }
    if (!hMod) {
        write_log("GameLogic.dll not found - aborting.");
        return 0;
    }

    // Compute addresses
    uintptr_t base = (uintptr_t)hMod;
    volatile int* health = (volatile int*)(base + OFF_HEALTH);
    volatile int* xp     = (volatile int*)(base + OFF_XP);
    volatile int* tech   = (volatile int*)(base + OFF_TECH);

    {
        char buf[256];
        snprintf(buf, sizeof(buf), "Module base: 0x%p, health ptr: 0x%p, xp ptr: 0x%p, tech ptr: 0x%p",
                 (void*)base, (void*)health, (void*)xp, (void*)tech);
        write_log(buf);
    }

    // Create indicator file so you can see the DLL hooked (in game folder)
    {
        std::ofstream ind(INDICATOR);
        if (ind) ind << "hooked\n";
    }

    // Show a message (optional) — may annoy some users but helps debug
    //MessageBoxA(NULL, "Progamingidea hook is running!", "DLL Loaded", MB_OK);

    // main loop: attempt to write only if the memory region looks OK
    while (true) {
        if (health && memory_is_writeable((void*)health)) {
            *health = 9999;
        }
        if (xp && memory_is_writeable((void*)xp)) {
            *xp = 9999;
        }
        if (tech && memory_is_writeable((void*)tech)) {
            *tech = 9999;
        }
        Sleep(100);
    }

    return 0;
}

// Small starter to avoid doing work inside DllMain loader lock
static DWORD WINAPI StarterThread(LPVOID) {
    Sleep(3000); // let the game finish some init (3s)
    CheatThread(NULL);
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        HANDLE h = CreateThread(NULL, 0, StarterThread, NULL, 0, NULL);
        if (h) CloseHandle(h);
    }
    return TRUE;
}
