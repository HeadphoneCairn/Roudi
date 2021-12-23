#pragma once

#include <Arduino.h> // for uint8_t

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
    uint8_t m_channel;                      // [0, 15]
    uint8_t m_minimum_note, m_maximum_note; // transmit notes in [m_minimum_note, m_maximum_note] range
    uint8_t m_minimum_velocity;             // only transmit notes with velocity >= m_minimum_velocity
    bool m_allow_pitch_modulation;          // whether to transmit pitch bend and modulation 
    int8_t m_transpose;                     // [-128, 127], transpose notes before transmitting  
  };

  class Configuration
  {
  public:
    Configuration();
    void SetDefaults();
    uint8_t m_input_channel;
    uint8_t m_nbr_output_channels;
    enum { m_max_number_of_output_channels = 2};
    OutputConfiguration m_output_channel[m_max_number_of_output_channels];
  };


  //==============================================================================
  // 
  //                      M I D I   P R O C E S S I N G
  //
  //==============================================================================

  // Initialize (must be called before any other function)
  void Init();
  // Change the midi processing
  // Returns true on success
  //         false if output queue was full, in which case the call should be 
  //                reattempted after writing the queue to MIDI out
  bool SetConfiguration(const Configuration& new_configuration);
  // Read from the input MIDI port, convert the messages and put them on the output queue
  void TreatInput();
  // Put the output messages on the output MIDI port
  void WriteToOutput();

}