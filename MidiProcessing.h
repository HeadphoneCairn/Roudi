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

#include <Arduino.h> // for uint8_t

#include "Data.h"

struct midi_event_t;

namespace MidiProcessing
{  
  //==============================================================================
  // 
  //  C O N F I G U R A T I O N   O F   T H E   M I D I   P R O C E S S I N G
  //
  //==============================================================================

  class OutputConfiguration
  {
  public:
    OutputConfiguration();
    void SetDefaults();
  public:    
    uint8_t m_channel;                      // [0, 15]
    uint8_t m_minimum_note, m_maximum_note; // transmit notes in [m_minimum_note, m_maximum_note] range
    uint8_t m_minimum_velocity;             // only transmit notes with velocity >= m_minimum_velocity
    uint8_t m_maximum_velocity;             // only transmit notes with velocity <= m_maximum_velocity
    bool m_allow_pitch_bend;                // if m_override_default_filter=true, whether to transmit pitch bend 
    bool m_allow_modulation;                // if m_override_default_filter=true, whether to transmit mod wheel
    bool m_allow_control_change;            // if m_override_default_filter=true, whether to transmit control change (CC's)
    bool m_map_pitch_bend_to_aftertouch;    // will convert pitch bend (if enabled) to aftertouch
    int8_t m_transpose;                     // [-128, 127], transpose notes before transmitting
  };

  class Configuration
  {
  public:
    Configuration();
    void SetDefaults();
  public:
    enum { m_max_number_of_output_channels = 2};
  public:
    uint8_t m_input_channel;
    uint8_t m_nbr_output_channels;
    FilterSettingsValues m_default_filter;
    bool m_override_default_filter;
    OutputConfiguration m_output_channel[m_max_number_of_output_channels];
  };


  //==============================================================================
  // 
  //                      M I D I   P R O C E S S I N G
  //
  //==============================================================================

  // Initialize (must be called before any other function)
  void Init();

  // Get the current active configuration
  const Configuration& GetConfiguration();
  // Changing the midi processing consists of two steps:
  // - Use SetNextConfiguration to specify the next configuration
  // - Call ActivateNextConfigurationIfAvailable until it returns true  
  void SetNextConfiguration(const Configuration& next_configuration);
  // ActivateNextConfiguration returns true if configuration was changed.
  bool ActivateNextConfigurationIfAvailable();

  // To be called to tell the main loop that panic has to be sent
  void SetPanic();
  // To be called in main loop.
  void SendPanicIfNeeded();

  // This interface can be used to get notified of every incoming MIDI message
  // WARNING: The midi_listener MUST return as quickly as possible!
  struct MidiListener {
    void (*call_back) (const midi_event_t& event, void* data);
    void* data;
  };
  void SetMidiInListener(const MidiListener& midi_listener);
  void SetMidiOutListener(const MidiListener& midi_listener);

  // Read from the input MIDI port, convert the messages and put them on the output queue
  void TreatInput();
  // Put the output messages on the output MIDI port
  void WriteToOutput();
}