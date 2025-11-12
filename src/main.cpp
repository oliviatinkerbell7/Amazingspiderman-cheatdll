#include <windows.h>
#include <fstream>

static void write_file(const char* path, const char* msg) {
    HANDLE h = CreateFileA(path,
                           GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote = 0;
    WriteFile(h, msg, (DWORD)strlen(msg), &wrote, NULL);
    CloseHandle(h);
}

DWORD WINAPI WorkerThread(LPVOID) {
    // Try a few locations the game / Winlator might use:
    write_file("dinput_cheat_log.txt", "HOOK: wrote to game folder\n");
    write_file("C:\\Users\\Public\\dinput_cheat_log.txt", "HOOK: wrote to C:\\Users\\Public\n");
    write_file("C:\\Temp\\dinput_cheat_log.txt", "HOOK: wrote to C:\\Temp\n");

    // If Winlator maps Android storage, try common Android paths (they'll fail on Windows but harmless)
    write_file("/sdcard/Download/dinput_cheat_log.txt", "HOOK: wrote to /sdcard/Download\n");
    write_file("/sdcard/Download/Telegram/dinput_cheat_log.txt", "HOOK: wrote to /sdcard/Download/Telegram\n");

    // Leave a tiny alive loop so we can tell if DLL stayed loaded for a bit
    for (int i=0;i<30;i++) {
        Sleep(500); // sleep so it won't hog CPU
    }
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        HANDLE h = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
        if (h) CloseHandle(h);
    }
    return TRUE;
}
