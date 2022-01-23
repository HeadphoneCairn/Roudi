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

const char* GetPStringEmpty()  { return GetPString(PSTR_empty); }
const char* GetPStringNone()   { return GetPString(PSTR_none); }


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


// --- Some (generic) value functions ---
PSTRING(PSTR_on_off_off, "off");
PSTRING(PSTR_on_off_on, "on");
const char* GetOnOff(uint8_t on)
{
  return GetPString(on ? PSTR_on_off_on : PSTR_on_off_off);
}

PSTRING(PSTR_numformat, "%d");
const char* GetNumberPlusOne(uint8_t value)
{
  sprintf(data_scratch, GetPString(PSTR_numformat), value + 1);
  return data_scratch;
}


//==============================================================================
// 
//                                 D A T A
//
//==============================================================================

void GetSingleDefault(struct SingleValues& values)
{
  memset(&values, 0, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
  values.channel = 0;
}

PSTRING(PSTR_default_multi_name, "Multi");
void GetMultiDefault(struct MultiValues& values)
{
  memset(&values, 0, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
  strcpy(values.name, GetPString(PSTR_default_multi_name));
  values.channel[1] = 1;
  values.octave[0] = values.octave[1] = OctaveDeltaToOctaveValue(0);
  values.split_note = 60; // C4
}

void GetSettingsDefault(struct SettingsValues& values)
{
  memset(&values, 0, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
  values.input_channel = 10;
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
  EEPROM has 1024 bytes:

  0000-0007: Header (8 bytes)
  0008-0049: Settings (32 bytes)
  0040-0215: Channels names: 16 x (10 chars + zero) (176 bytes)
  0216-0219: Single: 1 byte for selected line (=channel), 1 for first line (4 bytes)
  0220-0539: Multi x 10 (320 bytes)
              11 bytes (10 + 1) for the name 
               8 bytes (2 x 4) for channel settings
               2 bytes for mode
               3 bytes for selected line, selected field, first line
              = 24 bytes => for some future use 32 bytes 
  ... unused memory ...

  */

  static const uint16_t start_of_header = 0;
  static const uint16_t start_of_settings = 8;
  static const uint16_t start_of_channel_names = 40;
  static const uint16_t start_of_single = 216;
  static const uint16_t start_of_multi = 220;
  static const uint16_t multi_size = 32; // currently, we use 32 bytes for a multi
  static const uint8_t max_multis = 10; // TODO number_of_multis
  
  // We make sure to ring a bell when we save to the EEPROM to make
  // sure that we don't save too much by accident.
  // Because we can only save 10000 times to the EEPROM without damaging it.
  #define EEPROM_PUT(position, value) { Debug::BeepLow(); EEPROM.put(position, value); }
  #define EEPROM_GET(position, value)	{ EEPROM.get(position, value); }


  // ===== H E A D E R =========================================================

  struct EE_Header
  {
    uint16_t magic_number = 0x2B37;
    uint8_t version = 1;
    uint8_t number_of_multis = 1;
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

  // ===== M U L T I ===========================================================

  void SetMulti(uint8_t which, const struct MultiValues& values)
  {
    if (which < max_multis)
      EEPROM_PUT(start_of_multi + static_cast<uint16_t>(which) * multi_size, values);
  }
  
  void GetMulti(uint8_t which, struct MultiValues& values)
  {
    GetMultiDefault(values);
    if (which < max_multis)
      EEPROM_GET(start_of_multi + static_cast<uint16_t>(which) * multi_size, values);
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

  void SetChannelName(uint8_t channel_value, const char* channel_name)
  {
    if (channel_value < NumberOfChannels)
    {
      char name[MaxNameLength + 1];
      strncpy(name, channel_name, sizeof(name) - 1);
      name[sizeof(name) - 1] = 0;
      uint16_t position = start_of_channel_names + channel_value * sizeof(name);
      EEPROM_PUT(position, name);
    }
  }

  const char* GetChannelName(uint8_t channel_value)
  {
    *data_scratch = 0;
    if (channel_value < NumberOfChannels) {
      char name[MaxNameLength + 1];
      uint16_t position = start_of_channel_names + channel_value * sizeof(name);
      EEPROM_GET(position, name);
      strcpy(data_scratch, name);
    }
    return data_scratch;
  }

  PSTRING(PSTR_channel_formatted, "%02d. %s");
  const char* GetChannelNameFormatted(uint8_t channel_value)
  // Returns the name of the channel with a prefixed number 
  {
    char channel_name[MaxNameLength + 1];
    strcpy(channel_name, EE::GetChannelName(channel_value));
    sprintf(data_scratch, GetPString(PSTR_channel_formatted), channel_value + 1, channel_name);
    return data_scratch;
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
  
  void InitMulti()
  {
    struct MultiValues default_values;
    GetMultiDefault(default_values);
    SetMulti(0, default_values);
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
    for (uint8_t i = 0; i < NumberOfChannels; i++)
      EE::SetChannelName(i, GetPStringEmpty());
    // TODO: Temporary values that should be removed on release!
    EE::SetChannelName(0, GetPString(PSTR_channel_piano));
    EE::SetChannelName(4, GetPString(PSTR_channel_erebus));
    EE::SetChannelName(6, GetPString(PSTR_channel_typhon));
    EE::SetChannelName(9, GetPString(PSTR_channel_ipad));
    EE::SetChannelName(10, GetPString(PSTR_channel_prophet));
  }

  void Init()
  {
    uint16_t stored_magic_number;
    EEPROM_GET(start_of_header, stored_magic_number);
    EE_Header header;
    if (stored_magic_number != header.magic_number) {
      InitHeader();
      InitSingle();
      InitMulti();
      InitSettings();
      InitChannels();
    }
  }
}

