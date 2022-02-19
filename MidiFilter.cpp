#include "MidiFilter.h"


// --- Strings to be used in the pages ---
PSTRING(PSTR_filter_note_off,           "- Note off");
PSTRING(PSTR_filter_note_on,            "- Note on" );
PSTRING(PSTR_filter_note_on_off,        "- Note on/off"); // used as combination of on and off
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

static PSTRING(PSTR_MIDIMON_FILTER_MSGS, "Message filter");

Page::LineResult MessageFilterLine(Page::LineFunction func, FilterSettingsValues& filters)
{
  static uint8_t show = 0;

  Page::LineResult result = TextLine(func, PSTR_MIDIMON_FILTER_MSGS);
  if (func == Page::DO_LEFT || func == Page::DO_RIGHT) {
    show = 1 - show;
    memset(&filters, show, sizeof(filters));
    result.redraw = true;
  }
  return result;
}


