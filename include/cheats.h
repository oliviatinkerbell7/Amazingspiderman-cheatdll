#pragma once

// Offsets from GameLogic.dll (from your cheat table)
#define OFF_HEALTH 0x4E2230
#define OFF_XP     0x4F7B74
#define OFF_TECH   0x4F7B78

#ifdef _WIN32
#define LOGFILE "C:\\Users\\Public\\dinput_cheat_log.txt"
#else
#define LOGFILE "/sdcard/Download/dinput_cheat_log.txt"
#endif
