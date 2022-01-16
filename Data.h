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
const char* GetPStringUndo();
const char* GetPStringAccept();
const char* GetPStringMonitor();
const char* GetPStringLoadPreset(uint8_t preset);
const char* GetPStringSavePreset(uint8_t preset);

// --- Help for name menus ---
const char* GetPStringNameHelp1();
const char* GetPStringNameHelp2();
const char* GetPStringNameHelp3();

// --- Octaves ---
// Encapsulated the number of octaves to transpose
uint8_t GetNumberOfOctaves();
const char* GetOctaveName(uint8_t octave_value);
int8_t OctaveValueToOctaveDelta(uint8_t octave_value);
uint8_t OctaveDeltaToOctaveValue(int8_t octave_delta);

// --- MIDI Note names ---
const char* GetNoteName(uint8_t midi_note_number);


//==============================================================================
// 
//                                 D A T A
//
//==============================================================================

const uint8_t MaxNameLength = 15; // Max number of characters in channel name or preset name

struct SingleValues
{
  uint8_t channel; // saved to EEPROM as channel value (0-15) or menu entry (16-), shown as channel index (0-15) or menu entry (16-)
};
void GetSingleDefault(struct SingleValues& values);

struct SplitValues
{
  uint8_t left_channel;  // saved to EEPROM as channel value, shown as channel index 
  uint8_t left_octave;
  uint8_t right_channel;
  uint8_t right_octave;
  uint8_t split_note;
};
void GetSplitDefault(struct SplitValues& values);

struct LayerValues
{
  uint8_t a_channel;   // saved to EEPROM as channel value, shown as channel index
  uint8_t a_velocity;  // stored as 0 - 9, shown as 0% - 90%, interpreted as i*13 in range [0..127]
  uint8_t b_channel;
  uint8_t b_velocity;
};
void GetLayerDefault(struct LayerValues& values);

struct SettingsValues
{
  uint8_t input_channel;    // x // stored as 0-15, shown as 1-16
  uint8_t output_channels;
  uint8_t channel_order;
  uint8_t channel_number;
  uint8_t velocity_curve;   // x
  uint8_t program_change;   // x
  uint8_t split_delta;
  uint8_t brightness;       // x
};
void GetSettingsDefault(struct SettingsValues& values);



//==============================================================================
//
//                             E E P R O M
//
//==============================================================================


namespace EE
{
  // --- Init ---
  // Initiliaze EEPROM if first usage
  void Init();

  // --- Single ---
  void SetSingle(const struct SingleValues& values);
  void GetSingle(struct SingleValues& values);

  // --- Split ---
  void SetSplit(uint8_t preset, const struct SplitValues& values);
  void GetSplit(uint8_t preset, struct SplitValues& values);

  // --- Layer ---
  void SetLayer(uint8_t preset, const struct LayerValues& values);
  void GetLayer(uint8_t preset, struct LayerValues& values);

  // --- Settings ---
  void SetSettings(const struct SettingsValues& values);
  void GetSettings(struct SettingsValues& values);
  struct SettingsValues GetSettings();
  
  // --- Channels ---
  // channel index: index into current sorted representation of channels
  // channel value: 0-15
  // channel number: 1-16
  // channel name: Piano
  uint8_t GetNumberOfChannels();
  void SetChannelName(uint8_t channel_value, const char* channel_name);    
  const char* GetChannelName(uint8_t channel_value);
  // For channel iteration we use channel_index:
  const char* GetChannelNameFormatted(uint8_t channel_index);
  uint8_t ChannelIndexToChannelValue(uint8_t channel_index);
  uint8_t ChannelValueToChannelIndex(uint8_t channel_value);
}



const char* GetChannelNameBrol(uint8_t channel);
const char* GetOnOff(uint8_t on);
const char* GetNumberPlusOne(uint8_t value);