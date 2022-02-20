#include "MidiFilter.h"

#include "Debug.h"

#include <midi_serialization.h>


// --- Strings to be used in the pages ---
PSTRING(PSTR_filter_note_off,           "- Note off");      // Used in Monitor Settings
PSTRING(PSTR_filter_note_on,            "- Note on" );      // Used in Monitor Settings
PSTRING(PSTR_filter_note_on_off,        "- Note on/off");   // used as combination of on and off in Settings
PSTRING(PSTR_filter_key_pressure,       "- Key pressure");
PSTRING(PSTR_filter_control_change,     "- Control change");
PSTRING(PSTR_filter_program_change,     "- Program change");
PSTRING(PSTR_filter_channel_pressure,   "- Channel pressure");
PSTRING(PSTR_filter_pitch_bend,         "- Pitch bend");
PSTRING(PSTR_filter_system_exclusive,   "- System exclusive");
PSTRING(PSTR_filter_time_sync,          "- Time sync");
PSTRING(PSTR_filter_transport,          "- Transport");
PSTRING(PSTR_filter_active_sensing,     "- Active sensing");
PSTRING(PSTR_filter_other,              "- Other");

PSTRING(PSTR_filter_messages, "Messages:");

namespace MidiFilter
{
  bool AllowMessage(const FilterSettingsValues& filter, const midi_event_t& event)
  {
    switch (event.m_event) {
      case 0x8: return filter.note_off;
      case 0x9: return filter.note_on;
      case 0xA: return filter.key_pressure;
      case 0xB: return filter.control_change;
      case 0xC: return filter.program_change;    // might need be combined with bank select CC 00
      case 0xD: return filter.channel_pressure;
      case 0xE: return filter.pitch_bend;
      case 0xF:
        switch(event.m_data[0]) {
          case 0xF0:
          case 0xF7:
            return filter.system_exclusive;
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
      default: return true;
    };
  }
 
}