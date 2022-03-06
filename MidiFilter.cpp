#include "MidiFilter.h"

#include "Debug.h"
#include "MidiProcessing.h"

#include <midi_serialization.h>


// --- Strings to be used in the pages ---
PSTRING(PSTR_filter_note_off,           "- Note off");              // Used in Monitor Settings
PSTRING(PSTR_filter_note_on,            "- Note on" );              // Used in Monitor Settings
PSTRING(PSTR_filter_note_on_off,        "- Note on/off");           // Used as combination of On and Off in Settings
PSTRING(PSTR_filter_key_pressure,       "- Key pressure");
PSTRING(PSTR_filter_cc_all,             "- Control change");        // Used as combination of Mod Wheel and Other in Monitor Settings
PSTRING(PSTR_filter_cc_1_mod_wheel,     "- Mod wheel (CC#1)");      // Used in Settings
PSTRING(PSTR_filter_cc_other,           "- All other CC");          // Used in Settings
PSTRING(PSTR_filter_program_change,     "- Program change");
PSTRING(PSTR_filter_channel_pressure,   "- Channel pressure");
PSTRING(PSTR_filter_pitch_bend,         "- Pitch bend");
PSTRING(PSTR_filter_system_exclusive,   "- System exclusive");
PSTRING(PSTR_filter_time_sync,          "- Time sync");
PSTRING(PSTR_filter_transport,          "- Transport");
PSTRING(PSTR_filter_active_sensing,     "- Active sensing");
PSTRING(PSTR_filter_other,              "- Other");

namespace MidiFilter
{
  bool AllowMessage(const FilterSettingsValues& filter, const midi_event_t& event)
  {
    switch (event.m_event) {
      case 0x4:
      case 0x5:
      case 0x6:
      case 0x7: 
        return filter.system_exclusive;
      case 0x8: return filter.note_off;
      case 0x9: return filter.note_on;
      case 0xA: return filter.key_pressure;
      case 0xB: return (event.m_data[1] == 1 || event.m_data[1] == 1+32) ? filter.cc_mod_wheel : filter.cc_other;
      case 0xC: return filter.program_change;    // might need be combined with bank select CC 00
      case 0xD: return filter.channel_pressure;
      case 0xE: return filter.pitch_bend;
      default:
        switch(event.m_data[0]) {
          case 0xF1:
          case 0xF8:
            return filter.time_sync;
          case 0xF2:
          case 0xF3:
          case 0xFA:
          case 0xFB:
          case 0xFC:
            return filter.transport;
          case 0xFE:
            return filter.active_sensing;
          case 0xF6:
          case 0xFF:
            return filter.other;
          default:
            return true;
        }
    };
  }
 
  static bool IsChannel(const midi_event_t& event, uint8_t channel)
  {
    if (event.m_event >= 0x8 && event.m_event <= 0xe) {
      const uint8_t event_channel = event.m_data[0] & 0x0f;
      return (event_channel == channel);
    } else
      return true;    
  }

  bool IsActiveInputChannel(const midi_event_t& event)
  {
    return IsChannel(event, MidiProcessing::GetConfiguration().m_input_channel);
  }

  bool IsActiveOutputChannel(const midi_event_t& event)
  {
    const MidiProcessing::Configuration& config = MidiProcessing::GetConfiguration();
    for (uint8_t i = 0; i < config.m_nbr_output_channels; i++)
    {
      if (IsChannel(event, config.m_output_channel[i].m_channel))
        return true;
    }
    return false;
  }


}