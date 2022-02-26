#include "Data.h"
#include "Debug.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//==============================================================================
//
//                             P R O G M E M
//
//==============================================================================

static char progmem_string_buffer[30]; // for internal use, must have enough chars to fit all PSTRINGs!
char data_scratch[Screen::MaxCharsCanvas + 1]; // for internal an external use

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

// --- Octaves ---
#define MAX_OCTAVES 6
uint8_t GetNumberOfOctaves()
{
  return MAX_OCTAVES * 2 + 1;
}
PSTRING(PSTR_octave_0, "0");
PSTRING(PSTR_octave,   "%+d");
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

void GetSingleDefault(SingleValues& values)
{
  memset(&values, 0, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
  values.channel = 0;
}

PSTRING(PSTR_default_multi_name, "MULTI");
void GetMultiDefault(MultiValues& values)
{
  memset(&values, 0, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
  strcpy(values.name, GetPString(PSTR_default_multi_name));
  values.channel[1] = 1;
  values.octave[0] = values.octave[1] = OctaveDeltaToOctaveValue(0);
  values.split_note = 60; // C4
}

void GetFilterSettingsDefault(FilterSettingsValues& values)
{
  memset(&values, 1, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
}

void GetSettingsDefault(SettingsValues& values)
{
  memset(&values, 0, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
  GetFilterSettingsDefault(values.filter);
}

void GetMidiMonSettingsDefault(MidiMonSettingsValues& values)
{
  memset(&values, 0, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
  values.all_channels = 1;
  GetFilterSettingsDefault(values.filter);
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
  0008-0071: Settings: 4 bytes + 12 bytes (64 bytes)
  0072-0091: Midi Monitor Settings: 2 + 12 bytes (20 bytes)
  0092-0299: Channels names: 16 x (12 chars + zero) (208 bytes)
  0300-0307: Single: 1 byte for selected line (=channel), 1 for first line (8 bytes)
  0308-0311: Multi header: only number of multis for the moment (4 bytes)
  0312-0743: Multi x 12 (432 bytes)
              13 bytes (12 + 1) for the name 
               8 bytes (2 x 4) for channel settings
               2 bytes for mode
               3 bytes for selected line, selected field, first line
              = 26 bytes, but leave space for future use => 36 bytes 
  ... unused memory ...

  NOTE: EEPROM has 100,000 write/erase cycles
  */

  static const uint16_t start_of_header = 0;
  static const uint16_t start_of_settings = 8;
  static const uint16_t start_of_midimon_settings = 72;
  static const uint16_t start_of_channel_names = 92;
  static const uint16_t start_of_single = 300;
  static const uint16_t start_of_multi_header = 308;
  static const uint16_t start_of_multis = 312;
  static const uint16_t multi_size = 36; // currently, we use 36 bytes for a multi
  static const uint8_t  max_multis = 12; // currently, we have a max of 12 multis  TODO number_of_multis
  
//#define USE_SYSTEM_PUT_AND_GET
#ifdef USE_SYSTEM_PUT_AND_GET
  // This requires #include <EEPROM.h>.
  // It uses > 500 bytes of program storage space than my own routines. And I don't
  // really understand why.
  #define EEPROM_PUT(position, value) { Debug::Beep(); EEPROM.put(position, value); }
  #define EEPROM_GET(position, value)	{ EEPROM.get(position, value); }
#else
  void EepromGet(int idx, uint8_t* data, int data_size)
  {
    uint8_t *p = data;
    for (int count = 0; count < data_size; count++, p++)
      *p = eeprom_read_byte((uint8_t*) (idx + count));
  }

  void EepromPut(int idx, const uint8_t* data, int data_size)
  {
    // We make sure to ring a bell when we save to the EEPROM to make
    // sure that we don't save too much by accident.
    // Because we can only save 100,000 times to the EEPROM without damaging it.
    Debug::Beep();
    const uint8_t *p = data;
    for (int count = 0; count < data_size; count++, p++)
      eeprom_write_byte((uint8_t*) (idx + count), *p);
  }

  template<typename T>
  void EEPROM_GET(int idx, T &t)
  {
    EepromGet(idx, (uint8_t*) &t, sizeof(t));
  }

  template<typename T>
  void EEPROM_PUT(int idx, const T &t)
  {
    EepromPut(idx, (uint8_t*) &t, sizeof(t));
  }
#endif

  // ===== H E A D E R =========================================================

  struct EE_Header
  {
    uint16_t magic_number = 0x2B41;
    uint8_t version = 1;
  };

  // ===== S I N G L E =========================================================

  void SetSingle(const SingleValues& values)
  {
    EEPROM_PUT(start_of_single, values);
  }

  void GetSingle(SingleValues& values)
  {
    EEPROM_GET(start_of_single, values);
  }

  // ===== M U L T I ===========================================================

  struct EE_MultiHeader
  {
    uint8_t number_of_multis;
  };

  void SetNumberOfMultis(uint8_t number_of_multis)
  {
    EE_MultiHeader multi_header;
    multi_header.number_of_multis = number_of_multis;
    EEPROM_PUT(start_of_multi_header, multi_header);
  }

  uint8_t GetNumberOfMultis()
  {
    EE_MultiHeader multi_header;
    EEPROM_GET(start_of_multi_header, multi_header);
    return multi_header.number_of_multis;
  }

  uint8_t GetMaxNumberOfMultis()
  {
    return max_multis;
  }

  void SetMulti(uint8_t which, const MultiValues& values)
  {
    // This function saves the multi to the requested spot 'which'.
    // It is allowed to write one further than the current number of multis, 
    // in which case the number of multis is incremented.
    if (which <= GetNumberOfMultis() && which < GetMaxNumberOfMultis()) {
      if (which == GetNumberOfMultis()) // new multi
        SetNumberOfMultis(which + 1);
      EEPROM_PUT(start_of_multis + static_cast<uint16_t>(which) * multi_size, values);
    }
  }
  
  void GetMulti(uint8_t which, MultiValues& values)
  // I tried returning MultiValues instead having it as a output parameter,
  // but that actually generated quite a lot of extra program code (128 bytes for 4 calls!)
  // so I'm back to the output as a parameter way of working. 
  {
    GetMultiDefault(values);
    if (which < GetNumberOfMultis())
      EEPROM_GET(start_of_multis + static_cast<uint16_t>(which) * multi_size, values);
  }

  // ===== S E T T I N G S =====================================================

  SettingsValues g_settings_values; // Global variabel to have direct speedy access to the settings,
                                    // without having to read from the EEPROM

  SettingsValues& SettingsRW()
  {
    return g_settings_values;
  }

  const SettingsValues& Settings()
  {
    return g_settings_values;
  }

  void SetSettings() 
  {
    SettingsValues eeprom_values; 
    EEPROM_GET(start_of_settings, eeprom_values);
    if (memcmp(&eeprom_values, &g_settings_values, sizeof(eeprom_values)) != 0) 
      EEPROM_PUT(start_of_settings, g_settings_values);    
  }

  void GetSettings()
  {
    EEPROM_GET(start_of_settings, g_settings_values);
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

  PSTRING(PSTR_channel_formatted, "ch%02d. %s");
  const char* GetChannelNameFormatted(uint8_t channel_value)
  // Returns the name of the channel with a prefixed number 
  {
    char channel_name[MaxNameLength + 1];
    strcpy(channel_name, EE::GetChannelName(channel_value));
    sprintf(data_scratch, GetPString(PSTR_channel_formatted), channel_value + 1, channel_name);
    return data_scratch;
  }

  // ===== M I D I M O N S E T T I N G S =======================================

  void SetMidiMonSettings(const MidiMonSettingsValues& values)
  {
    EEPROM_PUT(start_of_midimon_settings, values);
  }

  void GetMidiMonSettings(MidiMonSettingsValues& values)
  {
    EEPROM_GET(start_of_midimon_settings, values);
  }

  // ===== I N I T =============================================================

  static void InitHeader()
  {
    EE_Header header;
    EEPROM_PUT(start_of_header, header);   
  }

  void InitSingle()
  {
    SingleValues default_values;
    GetSingleDefault(default_values);
    SetSingle(default_values);
  }
  
  void InitMulti()
  {
    // We create one 'default' multi
    SetNumberOfMultis(1);
    MultiValues default_values;
    GetMultiDefault(default_values);
    SetMulti(0, default_values);
  }

  static void InitSettings()
  {
    GetSettingsDefault(g_settings_values);
    EE::SetSettings();
  }

  static void InitMidiMonSettings()
  {
    MidiMonSettingsValues default_values;
    GetMidiMonSettingsDefault(default_values);
    SetMidiMonSettings(default_values);
  }

  PSTRING(PSTR_channel_piano,   "Piano");
  PSTRING(PSTR_channel_erebus,  "Erebus");
  PSTRING(PSTR_channel_typhon,  "Typhon");
  PSTRING(PSTR_channel_ipad,    "iPad");
  PSTRING(PSTR_channel_prophet, "Prophet 6");

  static void InitChannels()
  {
    for (uint8_t i = 0; i < NumberOfChannels; i++)
      EE::SetChannelName(i, GetPString(PSTR_empty));
    // TODO: Temporary values that should be removed on release!
    EE::SetChannelName(0, GetPString(PSTR_channel_piano));
    EE::SetChannelName(4, GetPString(PSTR_channel_erebus));
    EE::SetChannelName(6, GetPString(PSTR_channel_typhon));
    EE::SetChannelName(9, GetPString(PSTR_channel_ipad));
    EE::SetChannelName(10, GetPString(PSTR_channel_prophet));
  }

  void Init()
  {
    // Write default data to EEPROM, if there is no data yet
    uint16_t stored_magic_number;
    EEPROM_GET(start_of_header, stored_magic_number);
    EE_Header header;
    if (stored_magic_number != header.magic_number) {
      InitHeader();
      InitSingle();
      InitMulti();
      InitSettings();
      InitChannels();
      InitMidiMonSettings();
    }
    // Read the data that is constantly in memory for speedy access
    GetSettings();  
  }
}

