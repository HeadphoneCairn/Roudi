#pragma once

#include "Screen.h"

#include <avr/pgmspace.h>

//==============================================================================
//
//                             P R O G M E M
//
//==============================================================================

// Use these macros to define the strings and tables of strings to be put in PROGMEM
#define PSTRING(name, value) static const char name[] PROGMEM = value
#define PTABLE(name, ...)    static const char *const name[] PROGMEM = { __VA_ARGS__ }; \
                             static constexpr uint8_t name##_size = sizeof(name) / sizeof(*name);

// Use these functions to read the strings from PROGMEM
// The returned string remains in memory until the next call to GetPString... 
const char* GetPString(const char* pstring);
const char* GetPStringFromPTable(const char *const ptable[], uint8_t which);
//TODO Define PTableType???

// Scratch Pad for temporary string construnction
extern char data_scratch[Screen::MaxCharsCanvasComplete + 1];


//==============================================================================
// 
//                               V A L U E S
//
//==============================================================================

// --- Globally defined strings ---
const char* GetPStringEmpty();
const char* GetPStringNone();

// --- Octaves ---
// Encapsulated the number of octaves to transpose
uint8_t GetNumberOfOctaves();
const char* GetOctaveName(uint8_t octave_value);
int8_t OctaveValueToOctaveDelta(uint8_t octave_value);
uint8_t OctaveDeltaToOctaveValue(int8_t octave_delta);

// --- MIDI Note names ---
const char* GetNoteName(uint8_t midi_note_number);

// --- Some (generic) value functions ---
const char* GetOnOff(uint8_t on);
const char* GetNumberPlusOne(uint8_t value);

//==============================================================================
// 
//                                 D A T A
//
//==============================================================================

const uint8_t NumberOfChannels = 16;
const uint8_t MaxNameLength = 12; // Max number of characters in channel name or preset name, not including the /0

struct SingleValues
{
  uint8_t channel; // saved to EEPROM as channel (0-15)
};
void GetSingleDefault(SingleValues& values);

struct MultiValues
{
  uint8_t selected_line;  // used to recall original position of "cursor" 
  uint8_t selected_field; // used to recall original position of "cursor"
  uint8_t first_line;     // used to recall original position of "cursor"
  char name[MaxNameLength + 1]; // We also store the /0
  uint8_t channel[2];  // saved to EEPROM as channel (0-15)
  uint8_t octave[2];
  uint8_t pbcc[2];
  uint8_t velocity[2]; // stored as 0 - 9, shown as 0% - 90%, interpreted as i*13 in range [0..127]
  uint8_t mode;        // 0: SPLIT, 1: LAYER
  uint8_t split_note;
};
void GetMultiDefault(MultiValues& values);

struct SettingsValues
{
  uint8_t input_channel;    // stored as 0-15, shown as 1-16
  uint8_t velocity_curve;   // 
  uint8_t program_change;   // 
  uint8_t brightness;       // 
};
void GetSettingsDefault(SettingsValues& values);



//==============================================================================
//
//                             E E P R O M
//
//==============================================================================


namespace EE
{
  // --- Init ---
  // Initialize EEPROM if first usage
  void Init();

  // --- Single ---
  void SetSingle(const SingleValues& values);
  void GetSingle(SingleValues& values);

  // --- Multi ---
  uint8_t GetNumberOfMultis();
  uint8_t GetMaxNumberOfMultis();
  void SetMulti(uint8_t which, const MultiValues& values); // Only [0..GetNumberOfMultis] can be set, as long as GetNumberOfMultis()<GetMaxNumberOfMultis()
  void GetMulti(uint8_t which, MultiValues& values);

  // --- Settings ---
  void SetSettings(const SettingsValues& values);
  void GetSettings(SettingsValues& values);
  SettingsValues GetSettings();
  
  // --- Channels ---
  // channel value: 0-15
  // channel number: 1-16
  // channel name: Piano
  void SetChannelName(uint8_t channel_value, const char* channel_name);
  const char* GetChannelName(uint8_t channel_value);
  const char* GetChannelNameFormatted(uint8_t channel_value);
}


