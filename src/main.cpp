#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdint.h>

static const char *INDICATOR = "dinput_hooked.txt";

#ifdef _WIN32
static const char *LOGFILE = "C:\\Users\\Public\\dinput_cheat_log.txt";
#else
static const char *LOGFILE = "/sdcard/Download/Telegram/dinput_cheat_log.txt";
#endif

const uintptr_t OFF_HEALTH = 0x4E2230;
const uintptr_t OFF_XP     = 0x4F7B74;
const uintptr_t OFF_TECH   = 0x4F7B78;

void write_log(const char *s) {
    FILE *f = fopen(LOGFILE, "a");
    if (f) {
        fprintf(f, "%s\n", s);
        fclose(f);
    }
}

HMODULE WaitForModule(const char* name) {
    HMODULE h = NULL;
    while (!h) {
        h = GetModuleHandleA(name);
        if (!h) Sleep(200);
    }
    return h;
}

DWORD WINAPI CheatThread(LPVOID lpParam) {
    write_log("Cheat thread started, waiting for GameLogic.dll...");

    HMODULE hGame = WaitForModule("GameLogic.dll");
    if (!hGame) {
        write_log("Failed to find GameLogic.dll");
        return 0;
    }

    FILE *f = fopen(INDICATOR, "w");
    if (f) { fprintf(f, "hooked\n"); fclose(f); }

    uintptr_t base = (uintptr_t)hGame;
    int *pHealth = (int*)(base + OFF_HEALTH);
    int *pXP     = (int*)(base + OFF_XP);
    int *pTech   = (int*)(base + OFF_TECH);

    write_log("GameLogic.dll found, starting writes.");
    MessageBoxA(NULL, "Progamingidea hook is running!", "DLL Loaded", MB_OK);

    while (true) {
        __try {
            if (pHealth) *pHealth = 9999;
            if (pXP)     *pXP = 9999;
            if (pTech)   *pTech = 9999;
        } __except(EXCEPTION_EXECUTE_HANDLER) {}
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
