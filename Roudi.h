#pragma once

// --- Some global defines that can be used to configure Roudi ---

// Version shown on About Page
#define ROUDI_VERSION  "      version 1.1"
#define ROUDI_DATE     "       2019-2023"

// Enables showing of Headphone Cairn logo at startup. 
// Should be on on release.
//#define ENABLE_LOGO

// Enables showing the memory status in the About Page
// Should be off on release.
#define ENABLE_MEMORY_STATUS

// Enables debug printing and beeping.
// Should be off on release.
#define ENABLE_DEBUG

// Beeps every time that the eeprom is written to.
// Only works when ENABLE_DEBUG is on!
// Should be off on release.
#define ENABLE_WRITE_BEEP

// When eeprom is initialized, writes favorite channels names.
// Should be off on release.
#define ENABLE_DEFAULT_CHANNEL_NAMES

// Add the raw unfiltered input messages to the monitor.
// Should be off on release.
//#define ENABLE_MONITOR_RAW

// --- Uhm ---

// Ask main loop to redraw the current active Page
void SetRedrawNext();


