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

  uint8_t Velocity(uint8_t v_in);
  void Switch(uint8_t w);

}