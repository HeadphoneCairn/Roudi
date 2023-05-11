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


#include "MidiProcessing.h"

#include "Data.h"
#include "Debug.h"
#include "MidiFilter.h"
#include "Utils.h"
#include "Roudi.h"

#include "DinMidiboy.h"
#ifdef ENABLE_MIDI_OUTPUT_BUFFER
#include <fifo.h>
#endif
#include <midi_serialization.h>

namespace
{
  using namespace MidiProcessing;

  Configuration configuration;                 // currently active MIDI configuration
  Configuration g_next_configuration;          // next configuration, to be set if ...
  bool g_next_configuration_available = false; // ... next_confuration_available = true
  bool g_panic = false; // to be set if panic has to be sent to all channels 
  MidiListener g_midi_in_listener = {nullptr, nullptr};
  MidiListener g_midi_out_listener = {nullptr, nullptr};
  
#ifdef ENABLE_MIDI_OUTPUT_BUFFER
  typedef TFifo<midi_event_t, uint8_t, 128> fifo_t; // uses 512 bytes of RAM: 4 x 128

  fifo_t g_output_queue;
#endif
  MidiToUsb g_decoder;

  class State
  {
  public:
    State() { Init(); }
    void Init() {
      for (uint8_t i = 0; i < Configuration::m_max_number_of_output_channels; i++)
        m_active_note_on_count[i].SetAll(0);
    }
    Counters m_active_note_on_count[Configuration::m_max_number_of_output_channels];
  };

  State g_state;


  // === P R O C E S S I N G ===================================================

  // Fix for UsbToMidi::process() and 1 byte system common
  unsigned FIX_UsbToMidi_process(midi_event_t in, uint8_t out[3])
  {
    if (in.m_event == 0x1)
      in.m_event = 0x5;
    return UsbToMidi::process(in, out);
  }

  // Write directly to the midi out port
  void WriteEventToOutput(const midi_event_t& event)
  {
    uint8_t msg[3];
    uint8_t n = FIX_UsbToMidi_process(event, msg);
    for (uint8_t i=0; i<n; ++i)
      DinMidiboy.dinMidi().write(msg[i]);
    if (g_midi_out_listener.call_back)
      g_midi_out_listener.call_back(event, g_midi_out_listener.data);
  }

  void ProcessChannel(uint8_t channel, midi_event_t event
#ifdef ENABLE_MIDI_OUTPUT_BUFFER
    , fifo_t& output_queue
#endif
    )
  // This function takes an input event, transforms it and sends it to an output channel
  {
    // -- Init ---
    const OutputConfiguration& output_channel = configuration.m_output_channel[channel];
    Counters& output_channel_active_note_on_count = g_state.m_active_note_on_count[channel];

    // --- Filter message ---
    FilterSettingsValues filter = configuration.m_default_filter;
    if (configuration.m_override_default_filter) {
      filter.pitch_bend = output_channel.m_allow_pitch_bend;
      filter.cc_mod_wheel = output_channel.m_allow_modulation;
      filter.cc_other = output_channel.m_allow_control_change;
    }
    if (!MidiFilter::AllowMessage(filter, event))
      return;

    // --- Update channel ---
    event.m_data[0] = (event.m_data[0] & 0xf0) | (output_channel.m_channel & 0xf); 
    
    // --- Note on/off ---
    // Normally, every note ON is followed by a note OFF.
    // This code uses a velocity filter and as such does not send all note ONs to the output.
    // When -a bit later- the note OFF for such an event comes in, we don't know whether it
    // was filtered out or not, so we have to send it. However it is better to not send 
    // it to the output if it was filtered out. (At least I think that maybe some simple/older 
    // MIDI implementations might have problem with this.) Solution: keep track of the sent 
    // note ONs and only send a note OFF, when an ON was sent. This was originally implemented
    // with a simple true/false state, but I have noticed that some more advanced keyboards 
    // are capable of sending multiple note ONs before sending the note OFFs (required for
    // certain piano techniques). So I changed the tracking to a counter based system.
    // Typically the counters will not go over 1, but for fast repeated notes on a piano, it
    // can go higher. We support up to 15 of these notes max.  
    if (event.m_event == 0x9 || event.m_event == 0x8) { // Note ON or OFF
      const uint8_t note = event.m_data[1];
      if (note < output_channel.m_minimum_note || note > output_channel.m_maximum_note)
        return;
      const int16_t transposed_note_16 = static_cast<int16_t>(note) + static_cast<int16_t>(output_channel.m_transpose);
      if (transposed_note_16 < 0 || transposed_note_16 > 127)
        return;
      const uint8_t transposed_note = static_cast<uint8_t>(transposed_note_16);
      event.m_data[1] = transposed_note; // update the note in the event
      uint8_t velocity = event.m_data[2];
      if (event.m_event == 0x09 && velocity > 0) { // Note ON
        // Apply velocity map (or should we do it after the velocity filter?)
        velocity = MapVelocity(velocity);
        event.m_data[2] = velocity;
        // Filter based on velocity
        if (velocity < output_channel.m_minimum_velocity)
          return;
        if (velocity > output_channel.m_maximum_velocity)
          return;
        output_channel_active_note_on_count.Increment(transposed_note); // Increment number of active notes for this note
      } else { // Note Off (either 0x8 or 0x9 with velocity 0)
        if (output_channel_active_note_on_count.Get(transposed_note) == 0)
          return; // The note is not ON, so we don't send an OFF
        output_channel_active_note_on_count.Decrement(transposed_note); // Decrement number of active notes for this note
      }
    } else if (event.m_event == 0xE && output_channel.m_map_pitch_bend_to_aftertouch) { // Map pitch bend to channel pressure
      event.m_event = 0xD;
      event.m_data[0] = 0xD0 | (event.m_data[0] & 0x0F);
      if (event.m_data[2] == 64)
        event.m_data[1] = 0;
      else if (event.m_data[2] > 64)
        event.m_data[1] = (event.m_data[2] - 64) * 2 + 1;
      else // < 64
        event.m_data[1] = (64 - event.m_data[2]) * 2 - 1;
      event.m_data[2] = 0;
    }

    // --- Push the reformatted event
  #ifdef ENABLE_MIDI_OUTPUT_BUFFER
    output_queue.push(event);
  #else
    WriteEventToOutput(event);
  #endif
  }

  void ProcessInputEvent(midi_event_t& event
#ifdef ENABLE_MIDI_OUTPUT_BUFFER
  , fifo_t& output_queue
#endif
  )
  {
    if (event.m_event >= 0x8 && event.m_event <= 0xe) {
      // --- Event for a specific channel ---
      const uint8_t channel = event.m_data[0] & 0x0f;
      if (channel == configuration.m_input_channel) { // input channel
        for (uint8_t i = 0; i < configuration.m_nbr_output_channels; i++)
          ProcessChannel(i, event
#ifdef ENABLE_MIDI_OUTPUT_BUFFER      
          , output_queue
#endif
          );
      } else { // other channel
        if (!EE::Settings().block_other) // Should really also be in the Configuration, as should velocity_curve?????
#ifdef ENABLE_MIDI_OUTPUT_BUFFER  
          output_queue.push(event); // pass if not blocked in settings
#else
          WriteEventToOutput(event);
#endif
      }
    } else {
      // --- Event for all channels ---
      if (!MidiFilter::AllowMessage(configuration.m_default_filter, event))
        return;
#ifdef ENABLE_MIDI_OUTPUT_BUFFER
      output_queue.push(event); // just pass for the moment, later possible filtering
#else
      WriteEventToOutput(event);
#endif
    }
  }

  void SwitchOffAllNotes()
  // This started out as sending an "All Notes Off" message to all active output channels.
  // But not all synths treat this message correctly, e.g.:
  // - Dreadbox Erebus: stops all notes, but when you send a new note ON, it doesn't respond
  // - Dreadbox Typhon: does nothing when you send this meesage
  // So we now just send a NOTE off for each note that is on.
  // (I thought of just sending a note off for 0-127, but the piano keyboard can require
  //  more than one note off for the same note.)
#ifdef ENABLE_MIDI_OUTPUT_BUFFER
  //
  // WARNING: Writes directly to MIDI out, so should only be called after the output queue has
  //          been emptied, using WriteToOutput() !!!
  //          TODO: Maybe call WriteToOutput here?
#endif
  {    
    // For each channel, send note offs to all notes that are on 
    midi_event_t note_off_event;
    note_off_event.m_event = 0x8; // Note OFF
    note_off_event.m_data[2] = 64; // Typically unused velocity
    for (uint8_t i = 0; i < configuration.m_nbr_output_channels; i++) {
      note_off_event.m_data[0] = 0x80 | (configuration.m_output_channel[i].m_channel & 0x0f); // Note off on correct channel
      Counters& output_channel_active_note_on_count = g_state.m_active_note_on_count[i];
      for (uint8_t note = 0; note < 128; note++) {
        for (uint8_t active = 0; active < output_channel_active_note_on_count.Get(note); active++) {
          note_off_event.m_data[1] = note;    // Note number
          WriteEventToOutput(note_off_event); // Send the note off event
        }
      }
      output_channel_active_note_on_count.SetAll(0);
    }
  }

}


namespace MidiProcessing
{

  //==============================================================================
  // 
  //  C O N F I G U R A T I O N   O F   T H E   M I D I   P R O C E S S I N G
  //
  //==============================================================================

  OutputConfiguration::OutputConfiguration()
  {
    SetDefaults();
  }
  
  void OutputConfiguration::SetDefaults()
  {
    m_channel = 0;
    m_minimum_note = 0;
    m_maximum_note = 127;
    m_minimum_velocity = 0;
    m_maximum_velocity = 127;
    m_allow_pitch_bend = true;
    m_allow_modulation = true;
    m_allow_control_change = true;
    m_map_pitch_bend_to_aftertouch = false;
    m_transpose = 0;
  }

  Configuration::Configuration()
  {
    SetDefaults();
  }
  
  void Configuration::SetDefaults()
  {
    m_input_channel = 0;
    m_nbr_output_channels = 0;
    m_default_filter = EE::Settings().filter;
    m_override_default_filter = false;
    for (uint8_t i = 0; i < m_max_number_of_output_channels; ++i)
      m_output_channel[i].SetDefaults();
  }

  //==============================================================================
  // 
  //                      M I D I   P R O C E S S I N G
  //
  //==============================================================================

  void Init()
  {
    g_decoder.setCable(0); // is this really necessary?
    configuration.m_nbr_output_channels = 0;
/*
    configuration.m_nbr_output_channels = 2;
    configuration.m_output_channel[0].m_channel = 4; // 5
    configuration.m_output_channel[0].m_minimum_velocity = 80;
    configuration.m_output_channel[0].m_transpose = -2*12; // 10
    configuration.m_output_channel[1].m_channel = 9; // 10
    configuration.m_output_channel[1].m_minimum_note = 48; // C3
    configuration.m_output_channel[1].m_maximum_note = 72; // C5
    configuration.m_output_channel[1].m_transpose = 2*12; // 10
*/
  }

  const Configuration& GetConfiguration()
  {
    return configuration;
  }
   
  void SetNextConfiguration(const Configuration& next_configuration)
  {
    g_next_configuration_available = true;
    g_next_configuration = next_configuration;
  }

  bool ActivateNextConfigurationIfAvailable()
  {
    if (!g_next_configuration_available)
      return false;

    // TODO: We should return here if we're in the middle of sending SysEx.

    SwitchOffAllNotes();
    // The messages to switch of the notes on the output channels have been put on the output queue.
    // We can safely change the configuration.
    configuration = g_next_configuration;
    // Switch the velocity map
    SetVelocityCurve(EE::Settings().velocity_curve);
    // We are done
    g_next_configuration_available = false;
    return true;
  }

  void SetPanic()
  {
    g_panic = true;
  }

  void SendPanicIfNeeded()
#ifdef ENABLE_MIDI_OUTPUT_BUFFER
  // WARNING: Writes directly to MIDI out, so should only be called after the output queue has
  //          been emptied, using WriteToOutput() !!!
#endif
  {
    if (!g_panic)
      return;
 
    midi_event_t sound_off_event;
    sound_off_event.m_event = 0xb; // Control change
    sound_off_event.m_data[1] = 0x78; // All Sound Off
    sound_off_event.m_data[2] = 0x00; // unused
    for (uint8_t i = 0xb0; i < 0xc0; i++) {
      sound_off_event.m_data[0] = i; // Control change on correct channel
      WriteEventToOutput(sound_off_event); // Send the All Sound Off event
      }

    g_panic = false;
  }

  void SetMidiInListener(const MidiListener& midi_listener)
  {
    g_midi_in_listener = midi_listener;
  }

  void SetMidiOutListener(const MidiListener& midi_listener)
  {
    g_midi_out_listener = midi_listener;
  }

  /* 
  A word about g_decoder.process(), this function processes a MIDI input stream byte by byte.
  It returns eather false or true:
  - false: Not enough bytes have been treated to construct a complete midi_event_t. Please send more bytes.
  - true: The midi_event_t is ready, please treat the midi_event_t.
  To do this, it keeps state!

  A midi_event_t consists of the following parts:
  - m_event: the type of message as interpreted by g_decoder.process() from the bytes of the MIDI stream
	- m_data[3]: the actual bytes of the MIDI stream, depending on the type 1 to 3 bytes are set
  
  The function returns the following values for the m_event:
  - 0x2:     2 byte system common (F1=time code, F3=song select)
    0x3:     3 byte system common (F2=song position pointer)
    0x4:     sysex data
    0x5:     1 byte system common (F4, F6, F6=tune request) 
             => DAMNED!!! Should have been 0x1 => made local workaround, don't want to touch Blokas libs 
    0x5:     end of sysex data, F7 in m_data[0]
    0x6:     end of sysex data, F7 in m_data[1]
    0x7:     end of sysex data, F7 in m_data[2]
    0x8-0xE: the "channel" messages, such as key on/off, identical to midi command byte
    0xF:     midi real time (F8, FA, FB, FC, FE, FF)
  Note that the m_event has the possibility to store cables (in case you have many MIDI ports), 
  but we do not use that. 

  A word about system exclusive. Since these messages don't have fixed length, they are split up
  into a series of midi_event_t:
    0x4: F0, ??, ??
    0x4: ??, ??, ??
    ...
    0x4: ??, ??, ??
  It ends with one of the following
    0x5: F7, 00, 00
    0x6: ??, F7, 00
    0x7: ??, ??, F7
  Note that the in between these midi_events_t, system real time messages (0xF) may arrive!
  
  A word about running status, a method used by MIDI to reduce the number of bytes sent.
  Normally MIDI sends a command (=status) byte + several data bytes. If the next command is the same,
  MIDI allows the sending of just more data bytes. e.g. sending several NOTE ONs sequentially
  Note that NOTE ON with velocity 0 should be interpreted as NOTE OFF, which is especially useful 
  during running status. e.g. :
      90 3C 64  // NOTE 3C ON with velocity 64
      90 3D 65
      90 3E 5F
      80 3C 40  // NOTE 3C OF with release velocity 40
      80 3D 40
      80 3E 40
  Can be replaced by:
      90 3C 64
         3D 65
         3E 5F
         3C 00
         3D 00
         3E 00
  So 13 bytes instead of 18.
  g_decoder.process() supports the running status concept. Note however that it just returns an
  event for each bunch of data bytes, so you get a complete event for the 3E 00 above. 
  */

  // Fix for g_decoder.process() and 1 byte system common
  bool FIX_g_decoder_process(uint8_t byte, midi_event_t &out)
  {
    // 0x5 can be either a 1 byte system common (F4, F6, F6=tune request) message
    // or the end of system exclusive!
    // This is confusing. Would be better to use 0x1 for these 1 byte system common
    // WARNING: Don't know what the impact on latency is of this !!!!!!
    bool event_is_complete = g_decoder.process(byte, out);
    if (event_is_complete && out.m_event == 0x5 && out.m_data[0] != 0xF7)
      out.m_event = 0x1;
    return event_is_complete;
  }

  void TreatInput()
  {
    while (
#ifdef ENABLE_MIDI_OUTPUT_BUFFER      
      g_output_queue.hasSpaceFor(Configuration::m_max_number_of_output_channels /*number of involved output channels*/) && 
#endif
      DinMidiboy.dinMidi().available())
    {
      midi_event_t event;
      if (FIX_g_decoder_process(DinMidiboy.dinMidi().read(), event)) {
        if (g_midi_in_listener.call_back)
          g_midi_in_listener.call_back(event, g_midi_in_listener.data);
        ProcessInputEvent(event
#ifdef ENABLE_MIDI_OUTPUT_BUFFER      
        , g_output_queue
#endif
        );
      }
    }
  }

#ifdef ENABLE_MIDI_OUTPUT_BUFFER
  void WriteToOutput()
  {
    midi_event_t event;
    while (g_output_queue.pop(event))
      WriteEventToOutput(event);
  }
#endif

}