#pragma once
#include <cstdint>

// Log/indicator filenames (written in game's working directory — typically the game's folder)
#define LOGFILE "dinput_cheat_log.txt"
#define INDICATOR "dinput_hooked.txt"

// === Memory Offsets ===
// Replace these with the correct offsets for your game/version
#define OFF_HEALTH 0x4E2230
#define OFF_XP     0x4F7B74
#define OFF_TECH   0x4F7B78
