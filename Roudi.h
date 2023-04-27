#pragma once

// --- Some global defines that can be used to configure Roudi ---

// Version shown on About Page
#define ROUDI_VERSION  "      version 1.1"
#define ROUDI_DATE     "       2019-2023"

// Enables showing of Headphone Cairn logo at startup. 
// Should be ON on release.
// Uses about **1038** bytes of program space.
//#define ENABLE_LOGO

// Enables showing the memory status in the About Page
// Should be OFF on release.
// Uses about **534** bytes of program space.
#define ENABLE_MEMORY_STATUS

// Enables debug printing and beeping.
// Should be OFF on release.
// Uses about **820** bytes of program space.
#define ENABLE_DEBUG

// Beeps every time that the eeprom is written to.
// Only works when ENABLE_DEBUG is on!
// Should be OFF on release.
#define ENABLE_WRITE_BEEP

// When eeprom is initialized, writes favorite channels names.
// Should be OFF on release.
// Uses about **150** bytes of program space.
#define ENABLE_DEFAULT_CHANNEL_NAMES

// Add the raw unfiltered input messages to the monitor.
// Should be OFF on release.
// Uses about **200** bytes of program space.
//#define ENABLE_MONITOR_RAW

// The velocity edit page uses quit some program space.
// Since it is seldom used, it is possible to disable it.
// Uses about **1350** bytes of program space.
#define ENABLE_VELOCITY_EDIT_PAGE
// It is also possible to simplify the graphics and save space.
// Saves about **270** bytes of program space.
//#define ENABLE_VELOCITY_EDIT_PAGE_SIMPLE_GRAPHICS

// --- Uhm ---

// Ask main loop to redraw the current active Page
void SetRedrawNext();


