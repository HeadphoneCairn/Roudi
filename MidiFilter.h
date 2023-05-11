/*
 * Roudi, a ROUter for miDI messages for Blokas Midiboy
 * Copyright (C) 2019-2023 Headphone Cairn Software
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 2 of the License.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


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