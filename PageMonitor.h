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
#include "Utils.h"

#include <midi_serialization.h> // for midi_event_t

class PageMonitor: public Page
{
public:
  PageMonitor();
public: // For the listening interface
  struct midi_msg_t {
    midi_event_t event;
    uint8_t input:1;
  };
  static const uint8_t m_num_messages = 128; // Used to be 64, but was able to free more memory.
  TCircularBuffer<midi_msg_t, uint8_t, m_num_messages> m_messages;
  MidiMonSettingsValues m_settings;
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
private:
  LineResult LineDecode(const midi_msg_t& event);
};
