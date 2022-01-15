#include "Data.h"
#include "Debug.h"

#include <EEPROM.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//==============================================================================
//
//                             P R O G M E M
//
//==============================================================================

static char progmem_string_buffer[30]; // for internal use, must have enough chars to fit all PSTRINGs!
char data_scratch[Screen::MaxCharsCanvasComplete + 1]; // for internal an external use

const char* GetPString(const char* pstring)
{
  strncpy_P(progmem_string_buffer, pstring, sizeof(progmem_string_buffer)-1);
  progmem_string_buffer[sizeof(progmem_string_buffer)-1] = 0;
  return progmem_string_buffer;
}

const char* GetPStringFromPTable(const char *const ptable[], uint8_t which)
{
  const char* pstring = (const char *)pgm_read_word(&(ptable[which]));
  return GetPString(pstring);
}


//==============================================================================
// 
//                               V A L U E S
//
//==============================================================================

// --- Globally defined strings ---
PSTRING(PSTR_empty, "");
PSTRING(PSTR_none, "> None");
PSTRING(PSTR_undo, "> Undo");
PSTRING(PSTR_accept, "> Accept");
//PSTRING(PSTR_save, "> Save");
//PSTRING(PSTR_save_as, "> Save As");
PSTRING(PSTR_monitor, "> MIDI Monitor");

const char* GetPStringEmpty()  { return GetPString(PSTR_empty); }
const char* GetPStringNone()   { return GetPString(PSTR_none); }
const char* GetPStringUndo()   { return GetPString(PSTR_undo); }
const char* GetPStringAccept() { return GetPString(PSTR_accept); }
//const char* GetPStringSave()   { return GetPString(PSTR_save); }
//const char* GetPStringSaveAs() { return GetPString(PSTR_save_as); }
const char* GetPStringMonitor(){ return GetPString(PSTR_monitor); }

PSTRING(PSTR_load_preset, "> Load preset #%d");
PSTRING(PSTR_save_preset, "> Save as preset #%d");
const char* GetPStringLoadPreset(uint8_t preset) { sprintf(data_scratch, GetPString(PSTR_load_preset), preset); return data_scratch; }
const char* GetPStringSavePreset(uint8_t preset) { sprintf(data_scratch, GetPString(PSTR_save_preset), preset); return data_scratch; }

PSTRING(PSTR_name_help_1, "use left/right to move");
PSTRING(PSTR_name_help_2, "up/down to make changes");
PSTRING(PSTR_name_help_3, "v=accept, X=cancel");

// --- Help for name menus ---
const char* GetPStringNameHelp1()  { return GetPString(PSTR_name_help_1); }
const char* GetPStringNameHelp2()  { return GetPString(PSTR_name_help_2); }
const char* GetPStringNameHelp3()  { return GetPString(PSTR_name_help_3); }

// --- Octaves ---
#define MAX_OCTAVES 6
uint8_t GetNumberOfOctaves()
{
  return MAX_OCTAVES * 2 + 1;
}
PSTRING(PSTR_octave_0, "0");
PSTRING(PSTR_octave,   "%+d octave");
const char* GetOctaveName(uint8_t octave_value)
{
  const int8_t octave_delta = OctaveValueToOctaveDelta(octave_value);
  if (octave_delta == 0) // no transpose
    strcpy(data_scratch, GetPString(PSTR_octave_0));
  else // transpose by one or more octaves
    sprintf(data_scratch, GetPString(PSTR_octave), octave_delta);
  return data_scratch;
}
int8_t OctaveValueToOctaveDelta(uint8_t octave_value)
{
  return static_cast<int8_t>(octave_value) - MAX_OCTAVES;
}
uint8_t OctaveDeltaToOctaveValue(int8_t octave_delta)
{
  return static_cast<uint8_t>(octave_delta + MAX_OCTAVES);
}

// --- MIDI Note names ---
PSTRING(PSTR_note_name_00, "C%d");
PSTRING(PSTR_note_name_01, "C#%d");
PSTRING(PSTR_note_name_02, "D%d");
PSTRING(PSTR_note_name_03, "D#%d");
PSTRING(PSTR_note_name_04, "E%d");
PSTRING(PSTR_note_name_05, "F%d");
PSTRING(PSTR_note_name_06, "F#%d");
PSTRING(PSTR_note_name_07, "G%d");
PSTRING(PSTR_note_name_08, "G#%d");
PSTRING(PSTR_note_name_09, "A%d");
PSTRING(PSTR_note_name_10, "A#%d");
PSTRING(PSTR_note_name_11, "B%d");

PTABLE(PTAB_note_names, 
       PSTR_note_name_00, PSTR_note_name_01, PSTR_note_name_02, PSTR_note_name_03, 
       PSTR_note_name_04, PSTR_note_name_05, PSTR_note_name_06, PSTR_note_name_07, 
       PSTR_note_name_08, PSTR_note_name_09, PSTR_note_name_10, PSTR_note_name_11);

const char* GetNoteName(uint8_t midi_note_number)
// 0=C-1, 1=C#-1, ... 127=G9
{
  uint8_t note = midi_note_number % 12;
  int8_t octave = static_cast<int8_t>(midi_note_number / 12) - 1;
  sprintf(data_scratch, GetPStringFromPTable(PTAB_note_names, note), octave);
  return data_scratch;
}


//==============================================================================
// 
//                                 D A T A
//
//==============================================================================

void GetSingleDefault(struct SingleValues& values)
{
  values.channel = 0;
}

void GetSplitDefault(struct SplitValues& values)
{
  values.left_channel = 0;
  values.left_octave = OctaveDeltaToOctaveValue(0);
  values.right_channel = 0;
  values.right_octave = OctaveDeltaToOctaveValue(0);
  values.split_note = 60; // C4
}

void GetLayerDefault(struct LayerValues& values)
{
  memset(&values, 0, sizeof(values));
}

void GetSettingsDefault(struct SettingsValues& values)
{
  memset(&values, 0, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
  values.input_channel = 10;
  values.split_delta = 1;
}

//==============================================================================
//
//                             E E P R O M
//
//==============================================================================

namespace EE
{
  // ===== L A Y O U T =========================================================
  /*
  EPPROM has 1024 bytes:

  0000-0007: Header (8 bytes)
  0008-0023: Settings (16 bytes)
  0024-0039: Channels - alphabetical sorting (16 bytes)
  0040-0055:          - unused (16 bytes)
  0056-0311:          - names: 16 x (15 chars + zero) (256 bytes)
  0312-0315: Single (4 bytes)
  0316-0325: Split - auto-saved current (10 bytes)
  0326-0335:         preset #1 (10 bytes)
  0336-0345:         preset #2 (10 bytes)
  0346-0355:         preset #3 (10 bytes)
  0356-0365: Layer - auto-saved current (10 bytes)
  0366-0375:         preset #1 (10 bytes)
  0376-0385:         preset #2 (10 bytes)
  0386-0395:         preset #3 (10 bytes)
  ... unused memory ...

  // The original idea was to add menus with named presets:
  // 0480-0483: Presets - presets header (=number of presets) (4 bytes)
  // 0484-0503:         - preset order + type (split/layer) (20 bytes)
  // 0504-1023:         - 20 x (16 for name and 10 for values) (520 bytes)
  // but it became to complex, so added some simple presets to split and layer instead.
  */

  static const uint16_t start_of_header = 0;
  static const uint16_t start_of_settings = 8;
  static const uint16_t start_of_channel_order = 24;
  static const uint16_t start_of_channel_names = 56;
  static const uint16_t start_of_single = 312;
  static const uint16_t start_of_split = 316;
  static const uint16_t start_of_layer = 356;
  
  static const uint8_t max_presets = 3;
  static const uint16_t preset_size = 10; // currently, we use 10 bytes for a split or layer preset

  // We make sure to ring a bell when we save to the EEPROM to make
  // sure that we don't save too much by accident.
  // Because we can only save 10000 times to the EEPROM with damaging it.
  #define EEPROM_PUT(position, value) { Debug::BeepLow(); EEPROM.put(position, value); }
  #define EEPROM_GET(position, value)	{ EEPROM.get(position, value); }


  // ===== H E A D E R =========================================================

  struct EE_Header
  {
    uint16_t magic_number = 0x2B36;
    uint8_t version = 1;
  };

  // ===== S I N G L E =========================================================

  void SetSingle(const struct SingleValues& values)
  {
    EEPROM_PUT(start_of_single, values);
  }

  void GetSingle(struct SingleValues& values)
  {
    EEPROM_GET(start_of_single, values);
  }

  // ===== S P L I T ===========================================================

  // if preset = 0, we save to the location of the "current" value
  // else we save to the specified preset location
  void SetSplit(uint8_t preset, const struct SplitValues& values)
  {
    if (preset <= max_presets)
      EEPROM_PUT(start_of_split + static_cast<uint16_t>(preset) * preset_size, values);
  }
  
  void GetSplit(uint8_t preset, struct SplitValues& values)
  {
    GetSplitDefault(values);
    if (preset <= max_presets)
      EEPROM_GET(start_of_split + static_cast<uint16_t>(preset) * preset_size, values);
  }

  // ===== L A Y E R ===========================================================

  // if preset = 0, we save to the location of the "current" value
  // else we save to the specified preset location
  void SetLayer(uint8_t preset, const struct LayerValues& values)
  {
    if (preset <= max_presets)
      EEPROM_PUT(start_of_layer + static_cast<uint16_t>(preset) * preset_size, values);
  }

  void GetLayer(uint8_t preset, struct LayerValues& values)
  {
    GetLayerDefault(values);
    if (preset <= max_presets)
      EEPROM_GET(start_of_layer + static_cast<uint16_t>(preset) * preset_size, values);
  }

  // ===== S E T T I N G S =====================================================

  void SetSettings(const struct SettingsValues& values)
  {
    EEPROM_PUT(start_of_settings, values);
  }

  void GetSettings(struct SettingsValues& values)
  {
    EEPROM_GET(start_of_settings, values);
  }

  struct SettingsValues GetSettings()
  {
    struct SettingsValues values;
    EE::GetSettings(values);
    return values;
  }


  // ===== C H A N N E L S =====================================================

  static void SetChannelOrder(const uint8_t (&order_map)[16])
  {
    EEPROM_PUT(start_of_channel_order, order_map);
  }

  static void GetChannelOrder(uint8_t (&order_map)[16])
  {
    EEPROM_GET(start_of_channel_order, order_map);
  }

  static void SortChannels()
  {
    uint8_t channel_sort_map[16];
    // Put empty channels at the end of channel_sort_map (ordered according to channel value)
    // Put used channel at the begin of the channel_sort_map (not ordered)
    int8_t nbr_used = 0, empty_pos = 15;
    for (int8_t i = 15; i >= 0; i--) { // must be int8_t, because i will become -1
      if (GetChannelName(i)[0])
        channel_sort_map[nbr_used++] = i;
      else
        channel_sort_map[empty_pos--] = i;
    }
    // Sort the first nbr_used channels
    if (nbr_used > 0) {
      for (int8_t i = nbr_used - 1; i > 0; i--) {
        for (int8_t j = 0; j < i; j++) {
          char s1[MaxNameLength + 1], s2[MaxNameLength + 1];
          strcpy(s1, GetChannelName(channel_sort_map[j]));
          strcpy(s2, GetChannelName(channel_sort_map[j + 1]));
          if (strcasecmp(s1, s2) > 0) { // swap
            uint8_t temp = channel_sort_map[j];
            channel_sort_map[j] = channel_sort_map[j + 1];
            channel_sort_map[j + 1] = temp;
          }
        }
      }
    }
    // Save new order
    SetChannelOrder(channel_sort_map);
  }

  static void SetChannelNameRaw(uint8_t channel_value, const char* channel_name)
  {
    if (channel_value >= 16)
      return;
    char name[MaxNameLength + 1];
    strncpy(name, channel_name, sizeof(name) - 1);
    name[sizeof(name) - 1] = 0;
    uint16_t position = start_of_channel_names + channel_value * sizeof(name);
    EEPROM_PUT(position, name);
  }

  uint8_t GetNumberOfChannels()
  {
    return 16;
  }

  void SetChannelName(uint8_t channel_value, const char* channel_name)
  {
    SetChannelNameRaw(channel_value, channel_name);
    SortChannels();
  }

  const char* GetChannelName(uint8_t channel_value)
  {
    if (channel_value < 16) {
      char name[MaxNameLength + 1];
      uint16_t position = start_of_channel_names + channel_value * sizeof(name);
      EEPROM_GET(position, name);
      strcpy(data_scratch, name);
    } else {
      *data_scratch = 0;
    }
    return data_scratch;
  }

  PSTRING(PSTR_named_channel, ", %02d");
  PSTRING(PSTR_unnamed_channel, "ch%02d");

  const char* GetChannelNameFormatted(uint8_t channel_index)
  // Returns the name of the channel based on Settings.
  // Channel index is a number from 0..15 that is used
  // to iterate through the sorted channels.
  // To get the real channel value call ...
  {
    uint8_t channel_sort_map[16];
    EE::GetChannelOrder(channel_sort_map);

    const bool show_channel_number = EE::GetSettings().channel_number == 1;
    const bool sort_alphabetically = EE::GetSettings().channel_order == 0;
    const uint8_t channel_value = sort_alphabetically ? channel_sort_map[channel_index] : channel_index;
    char channel_name[MaxNameLength + 1];
    strcpy(channel_name, EE::GetChannelName(channel_value));
    if (*channel_name) {
      strcpy(data_scratch, channel_name);
      if (show_channel_number)
        sprintf(data_scratch + strlen(data_scratch), GetPString(PSTR_named_channel), channel_value + 1);
    } else {
      sprintf(data_scratch, GetPString(PSTR_unnamed_channel), channel_value + 1);
    }
    return data_scratch;
  }

  uint8_t ChannelIndexToChannelValue(uint8_t channel_index)
  // Converts index into current sorted channels into the real
  // channel value. Use this when saving the channel or when
  // acting upon the channel.
  {
    uint8_t channel_sort_map[16];
    EE::GetChannelOrder(channel_sort_map);

    if (channel_index < GetNumberOfChannels()) {
      const bool sort_alphabetically = EE::GetSettings().channel_order == 0;
      return sort_alphabetically ? channel_sort_map[channel_index] : channel_index; 
    } else {
      return 0; // should never happen
    }
  }

  uint8_t ChannelValueToChannelIndex(uint8_t channel_value)
  {
    uint8_t channel_sort_map[16];
    EE::GetChannelOrder(channel_sort_map);

    const bool sort_alphabetically = EE::GetSettings().channel_order == 0;
    if (sort_alphabetically) {
      for (uint8_t channel_index = 0; channel_index < GetNumberOfChannels(); ++channel_index) {
        if (channel_sort_map[channel_index] == channel_value)
          return channel_index;
      }
      return 0; // should never happen!
    } else {
      return channel_value;
    }   
  }

  // ===== I N I T =============================================================

  static void InitHeader()
  {
    EE_Header header;
    EEPROM_PUT(start_of_header, header);   
  }

  void InitSingle()
  {
    struct SingleValues default_values;
    GetSingleDefault(default_values);
    SetSingle(default_values);
  }
  
  void InitSplit()
  {
    struct SplitValues default_values;
    GetSplitDefault(default_values);
    for (uint8_t i = 0; i <= max_presets; i++)
      SetSplit(i, default_values);
  }

  void InitLayer()
  {
    struct LayerValues default_values;
    GetLayerDefault(default_values);
    for (uint8_t i = 0; i <= max_presets; i++)
      SetLayer(i, default_values);
  }

  static void InitSettings()
  {
    SettingsValues settings_values;
    GetSettingsDefault(settings_values);
    EE::SetSettings(settings_values);
  }

  PSTRING(PSTR_channel_piano,   "Piano");
  PSTRING(PSTR_channel_erebus,  "Erebus");
  PSTRING(PSTR_channel_typhon,  "Typhon");
  PSTRING(PSTR_channel_ipad,    "iPad");
  PSTRING(PSTR_channel_prophet, "Prophet 6");

  static void InitChannels()
  {
    for (uint8_t i = 0; i < 16; i++)
      EE::SetChannelNameRaw(i, GetPStringEmpty());
    // Temporary values that should be removed on release!
    EE::SetChannelNameRaw(0, GetPString(PSTR_channel_piano));
    EE::SetChannelNameRaw(4, GetPString(PSTR_channel_erebus));
    EE::SetChannelNameRaw(6, GetPString(PSTR_channel_typhon));
    EE::SetChannelNameRaw(9, GetPString(PSTR_channel_ipad));
    EE::SetChannelNameRaw(10, GetPString(PSTR_channel_prophet));
    SortChannels();
  }

  void Init()
  {
    uint16_t stored_magic_number;
    EEPROM_GET(start_of_header, stored_magic_number);
    EE_Header header;
    if (stored_magic_number != header.magic_number) {
      InitHeader();
      InitSingle();
      InitSplit();
      InitLayer();
      InitSettings();
      InitChannels();
    }
  }
}












PSTRING(PSTR_channel_00, "Piano");
PSTRING(PSTR_channel_01, "Moog");
PSTRING(PSTR_channel_02, "");
PSTRING(PSTR_channel_03, "Typhon");
PSTRING(PSTR_channel_04, "Er3bus");
PSTRING(PSTR_channel_05, "iPad");
PSTRING(PSTR_channel_06, "");
PSTRING(PSTR_channel_07, "");
PSTRING(PSTR_channel_08, "Computer");
PSTRING(PSTR_channel_09, "");
PSTRING(PSTR_channel_10, "");
PSTRING(PSTR_channel_11, "");
PSTRING(PSTR_channel_12, "");
PSTRING(PSTR_channel_13, "");
PSTRING(PSTR_channel_14, "");
PSTRING(PSTR_channel_15, "");

PTABLE(PTAB_channel_names,
       PSTR_channel_00,
       PSTR_channel_01,
       PSTR_channel_02,
       PSTR_channel_03,
       PSTR_channel_04,
       PSTR_channel_05,
       PSTR_channel_06,
       PSTR_channel_07,
       PSTR_channel_08,
       PSTR_channel_09,
       PSTR_channel_10,
       PSTR_channel_11,
       PSTR_channel_12,
       PSTR_channel_13,
       PSTR_channel_14,
       PSTR_channel_15);

PSTRING(PSTR_channel_name_brol_formatting, "%02d. ");

const char* GetChannelNameBrol(uint8_t channel)
{
  channel = channel % 16;
  sprintf(data_scratch, GetPString(PSTR_channel_name_brol_formatting), channel + 1);
  strcat(data_scratch, GetPStringFromPTable(PTAB_channel_names, channel));
  return data_scratch;
}


PSTRING(PSTR_on_off_off, "off");
PSTRING(PSTR_on_off_on, "on");
const char* GetOnOff(uint8_t on)
{
  return GetPString(on ? PSTR_on_off_on : PSTR_on_off_off);
}
