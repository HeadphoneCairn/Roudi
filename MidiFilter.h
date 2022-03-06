#pragma once

#include "Data.h"
#include "Page.h"

struct midi_event_t;

// --- Strings to be used in the pages ---

PSTRINGREF(PSTR_filter_note_off);
PSTRINGREF(PSTR_filter_note_on);
PSTRINGREF(PSTR_filter_note_on_off);
PSTRINGREF(PSTR_filter_key_pressure);
PSTRINGREF(PSTR_filter_cc_all);
PSTRINGREF(PSTR_filter_cc_1_mod_wheel);
PSTRINGREF(PSTR_filter_cc_other);
PSTRINGREF(PSTR_filter_program_change);
PSTRINGREF(PSTR_filter_channel_pressure);
PSTRINGREF(PSTR_filter_pitch_bend);
PSTRINGREF(PSTR_filter_system_exclusive);
PSTRINGREF(PSTR_filter_time_sync);
PSTRINGREF(PSTR_filter_transport);
PSTRINGREF(PSTR_filter_active_sensing);
PSTRINGREF(PSTR_filter_other);

namespace MidiFilter
{
  bool AllowMessage(const FilterSettingsValues& filter, const midi_event_t& event);
  bool IsActiveInputChannel(const midi_event_t& event);
  bool IsActiveOutputChannel(const midi_event_t& event);
}