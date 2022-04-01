#pragma once

// --- Some global defines that can be used to configure Roudi ---

// Enables showing of Headphone Cairn logo at startup. Should be on on release.
//#define ENABLE_LOGO
// Enables debug printing and beeping. Should be off on release.
#define ENABLE_DEBUG
// Beeps every time that the eeprom is written to. Should be off on release.
// Only works when ENABLE_DEBUG is on!
#define ENABLE_WRITE_BEEP
// When eeprom is initialized, writes favorite channels names. Should be off on release.
#define ENABLE_DEFAULT_CHANNEL_NAMES
// Add the raw unfiltered input messages to the monitor. Should be off on release.
//#define ENABLE_MONITOR_RAW


// --- Uhm ---

// Ask main loop to redraw the current active Page
void SetRedrawNext();


