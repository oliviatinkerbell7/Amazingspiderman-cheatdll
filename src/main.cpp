/*
  dinput8-style DLL skeleton for The Amazing Spider-Man 1
  - 32-bit DLL (compile as x86)
  - When loaded it spawns a thread that:
      * Waits for GameLogic.dll to appear
      * Writes fixed values to the offsets from your cheat table
      * Creates a small "dinput_hooked.txt" file in the working directory
  - Designed to be compiled in Windows (mingw or MSVC). Do NOT try to compile in Termux.
*/

#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

static const char *INDICATOR = "dinput_hooked.txt";
static const char *LOGFILE   = "dinput_cheat_log.txt";

// Offsets from your cheat table (GameLogic.dll + these)
const unsigned long OFF_HEALTH = 0x4E2230;
const unsigned long OFF_XP     = 0x4F7B74;
const unsigned long OFF_TECH   = 0x4F7B78;

// Values to write
const unsigned int VAL_HEALTH = 9999;
const unsigned int VAL_XP     = 999999;
const unsigned int VAL_TECH   = 9999;

void write_log(const char *s) {
    FILE *f = fopen(LOGFILE, "a");
    if (!f) return;
    SYSTEMTIME st; GetLocalTime(&st);
    fprintf(f, "[%02u:%02u:%02u] %s\n", st.wHour, st.wMinute, st.wSecond, s);
    fclose(f);
}

HMODULE find_module_by_name(const char* name) {
    DWORD pid = GetCurrentProcessId();
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snap == INVALID_HANDLE_VALUE) return NULL;
    MODULEENTRY32 me; me.dwSize = sizeof(me);
    if (Module32First(snap, &me)) {
        do {
            if (_stricmp(me.szModule, name) == 0) {
                HMODULE m = (HMODULE)me.hModule;
                CloseHandle(snap);
                return m;
            }
        } while (Module32Next(snap, &me));
    }
    CloseHandle(snap);
    return NULL;
}

void safe_write_u32(void* addr, unsigned int v) {
    DWORD old;
    if (VirtualProtect(addr, sizeof(v), PAGE_EXECUTE_READWRITE, &old)) {
        *(volatile unsigned int*)addr = v;
        VirtualProtect(addr, sizeof(v), old, &old);
    }
}

DWORD WINAPI worker(LPVOID) {
    write_log("Worker: started, waiting for GameLogic.dll...");
    HMODULE mod = NULL;
    int tries = 0;
    while (!mod && tries < 300) { // ~60s timeout
        mod = find_module_by_name("GameLogic.dll");
        if (!mod) { Sleep(200); tries++; }
    }
    if (!mod) {
        write_log("Worker: GameLogic.dll not found - exiting.");
        return 0;
    }

    char buff[256];
    sprintf(buff, "Worker: GameLogic.dll found at %p", mod); write_log(buff);

    uintptr_t base = (uintptr_t)mod;
    uintptr_t addr_hp  = base + OFF_HEALTH;
    uintptr_t addr_xp  = base + OFF_XP;
    uintptr_t addr_tech= base + OFF_TECH;

    sprintf(buff, "Writing health @ %p", (void*)addr_hp); write_log(buff);
    safe_write_u32((void*)addr_hp, VAL_HEALTH);

    sprintf(buff, "Writing xp @ %p", (void*)addr_xp); write_log(buff);
    safe_write_u32((void*)addr_xp, VAL_XP);

    sprintf(buff, "Writing tech @ %p", (void*)addr_tech); write_log(buff);
    safe_write_u32((void*)addr_tech, VAL_TECH);

    // Create indicator file in working directory (usually game folder)
    FILE *f = fopen(INDICATOR, "w");
    if (f) { fprintf(f, "Progamingidea hook is running\n"); fclose(f); write_log("Indicator file created."); }

    // Re-apply periodically for 2 minutes
    for (int i = 0; i < 120; ++i) {
        Sleep(1000);
        safe_write_u32((void*)addr_hp, VAL_HEALTH);
        safe_write_u32((void*)addr_xp, VAL_XP);
        safe_write_u32((void*)addr_tech, VAL_TECH);
    }

    write_log("Worker: finished.");
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinst);
        HANDLE h = CreateThread(NULL, 0, worker, NULL, 0, NULL);
        if (h) CloseHandle(h);
    }
    return TRUE;
}
