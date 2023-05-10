#pragma once

// --- Some global defines that can be used to configure Roudi ---

// Version shown on About Page
#define ROUDI_VERSION  "    version 1.1 beta"
#define ROUDI_DATE     "       2019-2023"

// Enables showing of Roudi logo at startup. 
// Should be ON on release.
// Uses about **522** bytes of program space.
//#define ENABLE_LOGO

// Enables showing the memory status in the About Page
// Should be OFF on release.
// Uses about **534** bytes of program space.
#define ENABLE_MEMORY_STATUS

// Enables debug printing and beeping.
// Should be OFF on release.
// Uses about **820** bytes of program space.
#define ENABLE_DEBUG

// We automatically save a page/menu after a certain period of inactivity (=not pushing buttons).
// To prevent writing to EEPROM too much during development, this timeout is quite high.
// Should be OFF on release.
//#define ENABLE_LONG_AUTOSAVE_TIMEOUT

// Beeps every time that the eeprom is written to.
// Only works when ENABLE_DEBUG is on!
// Should be OFF on release.
#define ENABLE_WRITE_BEEP

// When eeprom is initialized, writes the author's personal configuration:
// - channel names
// - velocity curves
// Should be OFF on release.
// Uses about **180** bytes of program space.
#define ENABLE_AUTHORS_CONFIGURATION

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

// For some reason that I cannot remember, I have an output buffer for the
// midi messages. I don't think it is needed, but before I remove it 
// permanently, I would like to do some more testing.
// Uses **512** bytes of RAM!
//#define ENABLE_MIDI_OUTPUT_BUFFER

// --- Uhm ---

// Ask main loop to redraw the current active Page
void SetRedrawNext();


