#include "Data.h"
#include "Debug.h"
#include "Roudi.h" // for #define ENABLE_WRITE_BEEP and ENABLE_AUTHORS_CONFIGURATION
#include "Utils.h"

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
PSTRING(PSTR_panic, "> Panic!");
PSTRING(PSTR_done, "> Done");
PSTRING(PSTR_velocity_curve_edit_title, " VELOCITY CURVE EDIT ");

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

// --- Velocity ---
uint8_t VelocityValueToVelocityMidi(uint8_t velocity_value)
{
  // input range is 0..21, output 0..127
  return velocity_value == 21 ? 127 : velocity_value * 6;
}

uint8_t VelocityMidiToVelocityValue(uint8_t velocity_value)
{
  return velocity_value / 6;
}

// --- Velocity Curves and Maps ---
/*
  To be clear about the terminology
  - A velocity curve is a complete mapping between all possible input velocity (1..127) and their expected output velocity.
    It is defined as the linear interpolation of a velocity map.
  - A velocity map is a list of 17 values that contains the output velocities for the following input velocities:
      1, 7, 15, 23, 31, 39, 47, 55, 63, 71, 79, 87, 95, 103, 111, 119, 127
  Remark: Velocity 0 is a special case. A NOTE ON with velocity 0 actually means NOTE OFF.
*/

static VelocityMap g_velocity_map;

PSTRING(PSTR_velocity_default, "linear");
PSTRING(PSTR_velocity_user, "user %d");

uint8_t GetNumberOfVelocityCurves() { return 4; }
const char* GetVelocityCurveName(uint8_t which) 
{
  if (which == 0 || which >= GetNumberOfVelocityCurves())
    return GetPString(PSTR_velocity_default);
  sprintf(data_scratch, GetPString(PSTR_velocity_user), which);
  return data_scratch; 
}

void SetVelocityCurve(uint8_t velocity_curve)
{
  EE::GetVelocityMap(velocity_curve, g_velocity_map);
}

void SetVelocityMap(const VelocityMap& velocity_map)
{
  memcpy(g_velocity_map, velocity_map, sizeof(VelocityMap));
}

uint8_t MapVelocity(uint8_t v_in)
{
  // Check input
  if (v_in == 0)
    return 0; // NOTE ON with velocity 0 has special NOTE OFF meaning, so never change!
  if (v_in > 127)
    v_in = 127; // safety precaution, 127 is max allowed velocity

  // Perform mapping, using linear interpolation
  const uint16_t from = v_in >> 3;
  const uint16_t from_out = static_cast<uint16_t>(g_velocity_map[from]);
  const uint16_t to_out = static_cast<uint16_t>(g_velocity_map[from+1]);
  const uint16_t from_in = (from << 3) - 1;
  const uint16_t to_in = (from << 3) + 7;
  uint16_t v_out = 0;
  if (from > 0)
    v_out = ((to_in - v_in) * from_out + (v_in - from_in) * to_out + 4) >> 3;
  else // special case interval 1..7
    v_out = ((to_in - v_in) * from_out + (v_in - 1) * to_out + 3) / 6;

  // Check output
  if (v_out == 0)
    v_out = 1; // output velocity 0 is only allowed when input velocity is 0!
  if (v_out > 127)
    v_out = 127; // safety precaution, 127 is max allowed velocity

  return static_cast<uint8_t>(v_out);
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
const char* GetNumber(uint8_t value)
{
  sprintf(data_scratch, GetPString(PSTR_numformat), value);
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
  values.split_note = 60; // C4
  values.channel[1].channel = 1;
  for (uint8_t i = 0; i < 2; i++) {
    values.channel[i].octave         = OctaveDeltaToOctaveValue(0);
    values.channel[i].max_velocity   = VelocityMidiToVelocityValue(127);
    values.channel[i].pitch_bend     = 1;
    values.channel[i].mod_wheel      = 1;
    values.channel[i].control_change = 1; 
  }
}

void GetFilterSettingsDefault(FilterSettingsValues& values)
{
  memset(&values, 1, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
}

void GetSettingsDefault(SettingsValues& values)
{
  memset(&values, 0, sizeof(values)); // sizeof of a reference gives the size of the referenced, so ok!
  values.brightness = Screen::BrightnessHigh;
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
  0008-0071: Settings: 4 bytes + 13 bytes (64 bytes)
  0072-0091: Midi Monitor Settings: 2 + 13 bytes (20 bytes)
  0092-0159: Velocity maps: 4 x 17 bytes (68 bytes)
  0160-0399: Channel names: 16 x (14 chars + zero) (240 bytes)
  0400-0407: Single: 1 byte for selected line (=channel), 1 for first line (8 bytes)
  0408-0411: Multi header: only number of multis for the moment (4 bytes)
  0412-0867: Multi x 12 (456 bytes)
               3 bytes for selected line, selected field, first line
              15 bytes (14 + zero) for the name 
              10 bytes (2 x 5) for channel settings
               2 bytes for mode
            = 30 bytes, but leave space for future use => 38 bytes 
  ... unused memory ...

  NOTE: EEPROM has 100,000 write/erase cycles
  */

  static const uint16_t start_of_header = 0;
  static const uint16_t start_of_settings = 8;
  static const uint16_t start_of_midimon_settings = 72;
  static const uint16_t start_of_velocity_maps = 92;
  static const uint16_t start_of_channel_names = 160;
  static const uint16_t start_of_single = 400;
  static const uint16_t start_of_multi_header = 408;
  static const uint16_t start_of_multis = 412;
  static const uint16_t multi_size = 38; // currently, we use 38 bytes for a multi
  static const uint8_t  max_multis = 12; // currently, we have a max of 12 multis
  
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
#ifdef ENABLE_WRITE_BEEP
    // We make sure to ring a bell when we save to the EEPROM to make
    // sure that we don't save too much by accident.
    // Because we can only save 100,000 times to the EEPROM without damaging it.
    Debug::Beep();
#endif
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
    // We only write if the data has been changed
    T t_read;
    EepromGet(idx, (uint8_t*) &t_read, sizeof(t));
    if (memcmp(&t, &t_read, sizeof(t)) != 0)
      EepromPut(idx, (uint8_t*) &t, sizeof(t));
  }
#endif

  // ===== H E A D E R =========================================================

  struct EE_Header
  {
    uint16_t magic_number = 0xAC11;
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

  uint8_t NewMulti()
  {
    uint8_t new_multi = EE::GetNumberOfMultis();
    if (new_multi < EE::GetMaxNumberOfMultis()) {
      MultiValues default_values;
      GetMultiDefault(default_values);
      SetMulti(new_multi, default_values);
      return new_multi;
    } else {
      return 0xFF;
    }
  }

  uint8_t RemoveMulti(uint8_t which) 
  {
    if (GetNumberOfMultis() > 1 && which < GetNumberOfMultis()) {
      for (uint8_t i = which + 1; i < GetNumberOfMultis(); i++) {
        MultiValues values;
        GetMulti(i, values);
        SetMulti(i - 1, values);
      }
      SetNumberOfMultis(GetNumberOfMultis() - 1);
    }
    return min(which, GetNumberOfMultis() - 1);
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

  PSTRING(PSTR_channel_formatted, "%02d. %s");
  PSTRING(PSTR_channel_formatted_ellipsis, "%02d. %-17s ...");
  const char* GetChannelNameFormatted(uint8_t channel_value, bool ellipsis)
  // Returns the name of the channel with a prefixed number 
  {
    char channel_name[MaxNameLength + 1];
    strcpy(channel_name, EE::GetChannelName(channel_value));
    sprintf(data_scratch, GetPString(ellipsis ? PSTR_channel_formatted_ellipsis : PSTR_channel_formatted), channel_value + 1, channel_name);
    return data_scratch;
  }

  // ===== M I D I M O N S E T T I N G S =======================================

  void __attribute__ ((noinline)) SetMidiMonSettings(const MidiMonSettingsValues& values)
  {
    EEPROM_PUT(start_of_midimon_settings, values);
  }

  void __attribute__ ((noinline)) GetMidiMonSettings(MidiMonSettingsValues& values)
  {
    EEPROM_GET(start_of_midimon_settings, values);
  }

  // ===== V E L O C I T Y M A P ===============================================

  void __attribute__ ((noinline)) SetVelocityMap(uint8_t which, const VelocityMap& velocity_map)
  {
    if (which < GetNumberOfVelocityCurves())
      EEPROM_PUT(start_of_velocity_maps + static_cast<uint16_t>(which) * sizeof(VelocityMap), velocity_map);
  }

  void __attribute__ ((noinline)) GetVelocityMap(uint8_t which, VelocityMap& velocity_map)
  {
    if (which >= GetNumberOfVelocityCurves())
      which = 0; // Fall back to default
    EEPROM_GET(start_of_velocity_maps + static_cast<uint16_t>(which) * sizeof(VelocityMap), velocity_map);
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
#ifdef ENABLE_AUTHORS_CONFIGURATION
    g_settings_values.velocity_curve = 1;
#endif
    EE::SetSettings();
  }

  static void InitMidiMonSettings()
  {
    MidiMonSettingsValues default_values;
    GetMidiMonSettingsDefault(default_values);
    SetMidiMonSettings(default_values);
  }

#ifdef ENABLE_AUTHORS_CONFIGURATION
  PSTRING(PSTR_channel_piano,   "Piano");
  PSTRING(PSTR_channel_typhon,  "Typhon");
  PSTRING(PSTR_channel_erebus,  "Erebus");
  PSTRING(PSTR_channel_prophet, "Prophet 6");
  PSTRING(PSTR_channel_volca,   "Volca Drums");
  PSTRING(PSTR_channel_ipad,    "iPad");
  PSTRING(PSTR_channel_pc,      "PC");
#endif

  static void InitChannels()
  {
    for (uint8_t i = 0; i < NumberOfChannels; i++)
      EE::SetChannelName(i, GetPString(PSTR_empty));
#ifdef ENABLE_AUTHORS_CONFIGURATION
    EE::SetChannelName(0, GetPString(PSTR_channel_piano));
    EE::SetChannelName(2, GetPString(PSTR_channel_typhon));
    EE::SetChannelName(3, GetPString(PSTR_channel_erebus));
    EE::SetChannelName(4, GetPString(PSTR_channel_prophet));
    EE::SetChannelName(5, GetPString(PSTR_channel_volca));
    EE::SetChannelName(6, GetPString(PSTR_channel_ipad));
    EE::SetChannelName(9, GetPString(PSTR_channel_pc));
#endif
  }

  const char velocity_map_linear[17] PROGMEM = {
      1,
      7,  15,  23,  31, 
     39,  47,  55,  63,
     71,  79,  87,  95, 
    103, 111, 119, 127 
  };

#ifdef ENABLE_AUTHORS_CONFIGURATION
  const char velocity_map_mypiano_at_medium_touch[17] PROGMEM = {
      1,
      1,   1,   2,  14, 
     28,  41,  51,  63,
     73,  84,  96,  108, 
    121, 127, 127,  127 
  };
#endif

  static void InitVelocityMaps()
  // Ugly hacked code to save 90 bytes of program space.
  // Normal way would be to use const PROGMEM uint8_t velocity_map_linear[17] = { ... } with memcpy_P.
  {
    strncpy_P(progmem_string_buffer, velocity_map_linear, 17);
    for (uint8_t i = 0; i < GetNumberOfVelocityCurves(); i++)
      SetVelocityMap(i, (VelocityMap&) progmem_string_buffer);
#ifdef ENABLE_AUTHORS_CONFIGURATION
    strncpy_P(progmem_string_buffer, velocity_map_mypiano_at_medium_touch, 17);
    SetVelocityMap(1, (VelocityMap&) progmem_string_buffer);
#endif
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
      InitVelocityMaps();
    }
    // Read the data that is constantly in memory for speedy access
    GetSettings();
  }
}

